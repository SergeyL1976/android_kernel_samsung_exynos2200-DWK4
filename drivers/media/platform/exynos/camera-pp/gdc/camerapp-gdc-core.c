/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Core file for Samsung EXYNOS ISPP GDC driver
 * (FIMC-IS PostProcessing Generic Distortion Correction driver)
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>

#ifdef CONFIG_EXYNOS_IOVMM
#include <linux/exynos_iovmm.h>
#else
#include <linux/io.h>
#include <linux/iommu.h>
#include <linux/dma-iommu.h>
#endif

#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-sg.h>
#include <media/v4l2-ioctl.h>

#if IS_ENABLED(CONFIG_EXYNOS_SCI)
#include <soc/samsung/exynos-sci.h>
#endif

#include "votf/camerapp-votf.h"
#include "camerapp-gdc.h"
#include "camerapp-hw-api-gdc.h"
#include "camerapp-video.h"

/* Flags that are set by us */
#define V4L2_BUFFER_MASK_FLAGS	(V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_QUEUED | \
				 V4L2_BUF_FLAG_DONE | V4L2_BUF_FLAG_ERROR | \
				 V4L2_BUF_FLAG_PREPARED | \
				 V4L2_BUF_FLAG_IN_REQUEST | \
				 V4L2_BUF_FLAG_REQUEST_FD | \
				 V4L2_BUF_FLAG_TIMESTAMP_MASK)
/* Output buffer flags that should be passed on to the driver */
#define V4L2_BUFFER_OUT_FLAGS	(V4L2_BUF_FLAG_PFRAME | V4L2_BUF_FLAG_BFRAME | \
				 V4L2_BUF_FLAG_KEYFRAME | V4L2_BUF_FLAG_TIMECODE)

static int gdc_log_level;
module_param_named(gdc_log_level, gdc_log_level, uint, S_IRUGO | S_IWUSR);

/*
 * If true, writes the latency of H/W operation to v4l2_buffer.reserved2
 * in the unit of nano seconds.  It must not be enabled with real use-case
 * because v4l2_buffer.reserved may be used for other purpose.
 * The latency is written to the destination buffer.
 */
static int __gdc_measure_hw_latency;
module_param_named(gdc_measure_hw_latency, __gdc_measure_hw_latency, int, 0644);

int __gdc_sfr_dump;
module_param_named(gdc_sfr_dump, __gdc_sfr_dump, int, 0644);

static int gdc_suspend(struct device *dev);
static int gdc_resume(struct device *dev);

struct vb2_gdc_buffer {
	struct v4l2_m2m_buffer mb;
	struct gdc_ctx *ctx;
	ktime_t ktime;
};

static struct device *gdc_device;

static const struct gdc_fmt gdc_formats[] = {
	{
		.name		= "YUV 4:2:0 non-contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV12M,
		.cfg_val	= GDC_CFG_FMT_YCBCR420_2P,
		.bitperpixel	= { 8, 4 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:0 non-contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV21M,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_2P,
		.bitperpixel	= { 8, 4 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:0 contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV12,
		.cfg_val	= GDC_CFG_FMT_YCBCR420_2P,
		.bitperpixel	= { 12 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:0 contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV21,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_2P,
		.bitperpixel	= { 12 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 packed, YCrYCb",
		.pixelformat	= V4L2_PIX_FMT_YVYU,
		.cfg_val	= GDC_CFG_FMT_YVYU,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 1,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 packed, YCbYCr",
		.pixelformat	= V4L2_PIX_FMT_YUYV,
		.cfg_val	= GDC_CFG_FMT_YUYV,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 1,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV16,
		.cfg_val	= GDC_CFG_FMT_YCBCR422_2P,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV61,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_2P,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 non-contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV16M,
		.cfg_val	= GDC_CFG_FMT_YCBCR422_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 non-contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV61M,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P010_16B",
		.pixelformat	= V4L2_PIX_FMT_NV12M_P010,
		.cfg_val	= GDC_CFG_FMT_P010_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P010_16B",
		.pixelformat	= V4L2_PIX_FMT_NV21M_P010,
		.cfg_val	= GDC_CFG_FMT_P010_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P210_16B",
		.pixelformat	= V4L2_PIX_FMT_NV16M_P210,
		.cfg_val	= GDC_CFG_FMT_P210_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P210_16B",
		.pixelformat	= V4L2_PIX_FMT_NV61M_P210,
		.cfg_val	= GDC_CFG_FMT_P210_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV422 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV16M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV422 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV61M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV420 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV12M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV420 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV21M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "NV12M SBWC LOSSY 8bit",
		.pixelformat	= V4L2_PIX_FMT_NV12M_SBWCL_8B,
		.cfg_val	= GDC_CFG_FMT_NV12M_SBWC_LOSSY_8B,
		.bitperpixel	= { 8, 4 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "NV12M SBWC LOSSY 10bit",
		.pixelformat	= V4L2_PIX_FMT_NV12M_SBWCL_10B,
		.cfg_val	= GDC_CFG_FMT_NV12M_SBWC_LOSSY_10B,
		.bitperpixel	= { 16, 8 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	},
};

int gdc_get_log_level(void)
{
	return gdc_log_level;
}

static int gdc_votf_set_service_config(int txs, struct gdc_dev *gdc)
{
	int ret = 0;
	int id = 0;
	struct votf_info vinfo;
	struct votf_service_cfg cfg;

	memset(&vinfo, 0, sizeof(struct votf_info));
	memset(&cfg, 0, sizeof(struct votf_service_cfg));

	if (txs) {
		/* TRS: Slave */
	} else {
		/* TWS: Master */
		for(id = 0; id < GDC_VOTF_ADDR_MAX; id++) {
			vinfo.service = TWS;
			vinfo.ip = gdc->votf_src_dest[GDC_SRC_ADDR] >> 16;
			vinfo.id = id;

			cfg.enable = 0x1;
			cfg.limit = 0x1;
			cfg.token_size = 0x1;
			cfg.connected_ip = gdc->votf_src_dest[GDC_DST_ADDR] >> 16;
			cfg.connected_id = id;
			cfg.option = 0;

			ret = votfitf_set_service_cfg_m_alone(&vinfo, &cfg);
			if (ret < 0) {
				ret = -EINVAL;
				gdc_dbg("votfitf_set_service_cfg for TWS is fail (src(%d, %d), dst(%d, %d))",
					vinfo.ip, vinfo.id, cfg.connected_ip, cfg.connected_id);
			} else
				gdc_dbg("votfitf_set_service_cfg success (src(0x%x, %d), dst(0x%x, %d))",
					vinfo.ip, vinfo.id, cfg.connected_ip, cfg.connected_id);

		}
	}

	return ret;
}

static int gdc_votfitf_set_flush(struct gdc_dev *gdc)
{
	int ret = 0;
	int id = 0;
	struct votf_info vinfo;

	memset(&vinfo, 0, sizeof(struct votf_info));

	for(id = 0; id < GDC_VOTF_ADDR_MAX; id++) {
		vinfo.service = TWS;
		vinfo.ip = gdc->votf_src_dest[GDC_SRC_ADDR] >> 16;
		vinfo.id = id;

		ret = votfitf_set_flush_alone(&vinfo);
		if (ret < 0)
			gdc_dbg("votfitf_set_flush failed (src(0x%x, %d)",
				vinfo.ip, vinfo.id);
	}
	return ret;
}

/* Find the matches format */
static const struct gdc_fmt *gdc_find_format(struct gdc_dev *gdc,
						u32 pixfmt, bool output_buf)
{
	const struct gdc_fmt *gdc_fmt;
	unsigned long i;

	for (i = 0; i < ARRAY_SIZE(gdc_formats); ++i) {
		gdc_fmt = &gdc_formats[i];
		if (gdc_fmt->pixelformat == pixfmt) {
			return &gdc_formats[i];
		}
	}

	return NULL;
}

static int gdc_v4l2_querycap(struct file *file, void *fh,
			     struct v4l2_capability *cap)
{
	strncpy(cap->driver, GDC_MODULE_NAME, sizeof(cap->driver) - 1);
	strncpy(cap->card, GDC_MODULE_NAME, sizeof(cap->card) - 1);

	cap->capabilities = V4L2_CAP_STREAMING |
		V4L2_CAP_VIDEO_CAPTURE_MPLANE | V4L2_CAP_VIDEO_OUTPUT_MPLANE;
	cap->capabilities |= V4L2_CAP_DEVICE_CAPS;
	cap->device_caps = V4L2_CAP_VIDEO_M2M_MPLANE;

	return 0;
}

static int gdc_v4l2_g_fmt_mplane(struct file *file, void *fh,
			  struct v4l2_format *f)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	const struct gdc_fmt *gdc_fmt;
	struct gdc_frame *frame;
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	int i;

	frame = ctx_get_frame(ctx, f->type);
	if (IS_ERR(frame))
		return PTR_ERR(frame);

	gdc_fmt = frame->gdc_fmt;

	pixm->width		= frame->width;
	pixm->height		= frame->height;
	pixm->pixelformat	= frame->pixelformat;
	pixm->field		= V4L2_FIELD_NONE;
	pixm->num_planes	= frame->gdc_fmt->num_planes;
	pixm->colorspace	= 0;

	for (i = 0; i < pixm->num_planes; ++i) {
		pixm->plane_fmt[i].bytesperline = (pixm->width *
				gdc_fmt->bitperpixel[i]) >> 3;
		if (gdc_fmt_is_ayv12(gdc_fmt->pixelformat)) {
			unsigned int y_size, c_span;
			y_size = pixm->width * pixm->height;
			c_span = ALIGN(pixm->width >> 1, 16);
			pixm->plane_fmt[i].sizeimage =
				y_size + (c_span * pixm->height >> 1) * 2;
		} else {
			pixm->plane_fmt[i].sizeimage =
				pixm->plane_fmt[i].bytesperline * pixm->height;
		}

		v4l2_dbg(1, gdc_log_level, &ctx->gdc_dev->m2m.v4l2_dev,
				"[%d] plane: bytesperline %d, sizeimage %d\n",
				i, pixm->plane_fmt[i].bytesperline,
				pixm->plane_fmt[i].sizeimage);
	}

	return 0;
}

static int gdc_v4l2_try_fmt_mplane(struct file *file, void *fh,
			    struct v4l2_format *f)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	const struct gdc_fmt *gdc_fmt;
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	const struct gdc_size_limit *limit;
	struct gdc_frame *frame;
	int i;
	int h_align = 0;
	int w_align = 0;

	if (!V4L2_TYPE_IS_MULTIPLANAR(f->type)) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
				"not supported v4l2 type\n");
		return -EINVAL;
	}

	gdc_fmt = gdc_find_format(ctx->gdc_dev, f->fmt.pix_mp.pixelformat, V4L2_TYPE_IS_OUTPUT(f->type));
	if (!gdc_fmt) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
				"not supported format type\n");
		return -EINVAL;
	}

	frame = ctx_get_frame(ctx, f->type);
	if (IS_ERR(frame))
		return PTR_ERR(frame);

	if (V4L2_TYPE_IS_OUTPUT(f->type))
		limit = &ctx->gdc_dev->variant->limit_input;
	else
		limit = &ctx->gdc_dev->variant->limit_output;

/* TODO: check */
	w_align = gdc_fmt->h_shift;
	h_align = gdc_fmt->v_shift;

	/* Bound an image to have width and height in limit */
	v4l_bound_align_image(&pixm->width, limit->min_w, limit->max_w,
			w_align, &pixm->height, limit->min_h,
			limit->max_h, h_align, 0);
/**/
	for (i = 0; i < gdc_fmt->num_planes; ++i) {
		/* The pixm->plane_fmt[i].sizeimage for the plane which
		 * contains the src blend data has to be calculated as per the
		 * size of the actual width and actual height of the src blend
		 * buffer */

		pixm->plane_fmt[i].bytesperline = (pixm->width *
						gdc_fmt->bitperpixel[i]) >> 3;
		if (gdc_fmt_is_ayv12(gdc_fmt->pixelformat)) {
			unsigned int y_size, c_span;
			y_size = pixm->width * pixm->height;
			c_span = ALIGN(pixm->width >> 1, 16);
			pixm->plane_fmt[i].sizeimage =
				y_size + (c_span * pixm->height >> 1) * 2;
		} else {
			if ((gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_S10B)
				|| (gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_S10B)) {
				if ((i % 2) == 0)
					pixm->plane_fmt[i].sizeimage =
						NV12M_Y_SIZE(pixm->width, pixm->height)
						+ NV12M_Y_2B_SIZE(pixm->width, pixm->height);
				else if ((i % 2) == 1)
					pixm->plane_fmt[i].sizeimage =
						NV12M_CBCR_SIZE(pixm->width, pixm->height)
						+ NV12M_CBCR_2B_SIZE(pixm->width, pixm->height);
			} else if ((gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_S10B)
				|| (gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_S10B)) {
				if ((i % 2) == 0)
					pixm->plane_fmt[i].sizeimage =
						NV16M_Y_SIZE(pixm->width, pixm->height)
						+ NV16M_Y_2B_SIZE(pixm->width, pixm->height);
				else if ((i % 2) == 1)
					pixm->plane_fmt[i].sizeimage =
						NV16M_CBCR_SIZE(pixm->width, pixm->height)
						+ NV16M_CBCR_2B_SIZE(pixm->width, pixm->height);
			} else if ((gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_P010)
				|| (gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_P010)) {
				if ((i % 2) == 0)
					pixm->plane_fmt[i].sizeimage =
						ALIGN(pixm->plane_fmt[i].bytesperline, 16) * pixm->height;
				else if ((i % 2) == 1)
					pixm->plane_fmt[i].sizeimage =
						(ALIGN(pixm->plane_fmt[i].bytesperline, 16) * pixm->height) >> 1;
			} else if ((gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_P210)
				|| (gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_P210)) {
				pixm->plane_fmt[i].sizeimage =
					ALIGN(pixm->plane_fmt[i].bytesperline, 16) * pixm->height;
			} else if (gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_SBWCL_8B) {
				frame->comp_rate = camerapp_hw_get_comp_rate(ctx->gdc_dev,
							gdc_fmt->pixelformat);
				if ((i % 2) == 0)
					pixm->plane_fmt[i].sizeimage =
						SBWCL_8B_Y_SIZE(pixm->width, pixm->height,
						frame->comp_rate);
				else if ((i % 2) == 1)
					pixm->plane_fmt[i].sizeimage =
						SBWCL_8B_CBCR_SIZE(pixm->width, pixm->height,
						frame->comp_rate);
			} else if (gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_SBWCL_10B) {
				frame->comp_rate = camerapp_hw_get_comp_rate(ctx->gdc_dev,
							gdc_fmt->pixelformat);
				if ((i % 2) == 0)
					pixm->plane_fmt[i].sizeimage =
						SBWCL_10B_Y_SIZE(pixm->width, pixm->height,
							frame->comp_rate);
				else if ((i % 2) == 1)
					pixm->plane_fmt[i].sizeimage =
						SBWCL_10B_CBCR_SIZE(pixm->width, pixm->height,
							frame->comp_rate);
			} else {
				pixm->plane_fmt[i].sizeimage =
					pixm->plane_fmt[i].bytesperline * pixm->height;
			}
		}

		v4l2_dbg(1, gdc_log_level, &ctx->gdc_dev->m2m.v4l2_dev,
				"[%d] plane: bytesperline %d, sizeimage %d\n",
				i, pixm->plane_fmt[i].bytesperline,
				pixm->plane_fmt[i].sizeimage);
	}

	return 0;
}

static int gdc_v4l2_s_fmt_mplane(struct file *file, void *fh,
				 struct v4l2_format *f)

{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	struct vb2_queue *vq = v4l2_m2m_get_vq(ctx->m2m_ctx, f->type);
	struct gdc_frame *frame;
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	const struct gdc_size_limit *limitout =
				&ctx->gdc_dev->variant->limit_input;
	const struct gdc_size_limit *limitcap =
				&ctx->gdc_dev->variant->limit_output;
	int i, ret = 0;
	gdc_dbg("gdc s_fmt_mplane\n");

	if (vb2_is_streaming(vq)) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev, "device is busy\n");
		return -EBUSY;
	}

	ret = gdc_v4l2_try_fmt_mplane(file, fh, f);
	if (ret < 0)
		return ret;

	frame = ctx_get_frame(ctx, f->type);
	if (IS_ERR(frame))
		return PTR_ERR(frame);

	set_bit(CTX_PARAMS, &ctx->flags);

	frame->gdc_fmt = gdc_find_format(ctx->gdc_dev, f->fmt.pix_mp.pixelformat, V4L2_TYPE_IS_OUTPUT(f->type));
	if (!frame->gdc_fmt) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
				"not supported format values\n");
		return -EINVAL;
	}

	frame->num_planes = (pixm->num_planes < GDC_MAX_PLANES) ? pixm->num_planes : GDC_MAX_PLANES;

	for (i = 0; i < frame->num_planes; i++) {
		if (i < frame->gdc_fmt->num_planes)
			frame->bytesused[i] = pixm->plane_fmt[i].sizeimage;
		else
			frame->bytesused[i] = ALIGN(sizeof(struct gdc_metadata), 16);
	}

	if (V4L2_TYPE_IS_OUTPUT(f->type) &&
		((pixm->width > limitout->max_w) ||
			 (pixm->height > limitout->max_h))) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
			"%dx%d of source image is not supported: too large\n",
			pixm->width, pixm->height);
		return -EINVAL;
	}

	if (!V4L2_TYPE_IS_OUTPUT(f->type) &&
		((pixm->width > limitcap->max_w) ||
			 (pixm->height > limitcap->max_h))) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
			"%dx%d of target image is not supported: too large\n",
			pixm->width, pixm->height);
		return -EINVAL;
	}

	if (V4L2_TYPE_IS_OUTPUT(f->type) &&
		((pixm->width < limitout->min_w) ||
			 (pixm->height < limitout->min_h))) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
			"%dx%d of source image is not supported: too small\n",
			pixm->width, pixm->height);
		return -EINVAL;
	}

	if (!V4L2_TYPE_IS_OUTPUT(f->type) &&
		((pixm->width < limitcap->min_w) ||
			 (pixm->height < limitcap->min_h))) {
		v4l2_err(&ctx->gdc_dev->m2m.v4l2_dev,
			"%dx%d of target image is not supported: too small\n",
			pixm->width, pixm->height);
		return -EINVAL;
	}

	frame->width = pixm->width;
	frame->height = pixm->height;
	frame->pixelformat = pixm->pixelformat;

	/* Set the SBWC flag */
	frame->pixel_size = (pixm->flags & CAMERAPP_PIXEL_SIZE_MASK) >> CAMERAPP_PIXEL_SIZE_SHIFT;
	frame->extra = (pixm->flags & CAMERAPP_EXTRA_MASK) >> CAMERAPP_EXTRA_SHIFT;
	gdc_dbg("pixelformat(%c%c%c%c) size(%dx%d) pixel_size(%d) extra(%d) lossy_rate(%d)\n",
		(char)((frame->gdc_fmt->pixelformat >> 0) & 0xFF),
		(char)((frame->gdc_fmt->pixelformat >> 8) & 0xFF),
		(char)((frame->gdc_fmt->pixelformat >> 16) & 0xFF),
		(char)((frame->gdc_fmt->pixelformat >> 24) & 0xFF),
		frame->width, frame->height,
		frame->pixel_size, frame->extra, frame->comp_rate);

	/* Check constraints for SBWC */
	if (frame->extra && camerapp_hw_get_sbwc_constraint(frame, f->type))
		return -EINVAL;

	return 0;
}

static int gdc_v4l2_reqbufs(struct file *file, void *fh,
			    struct v4l2_requestbuffers *reqbufs)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	gdc_dbg("v4l2_reqbuf\n");

	return v4l2_m2m_reqbufs(file, ctx->m2m_ctx, reqbufs);
}

static int gdc_v4l2_querybuf(struct file *file, void *fh,
			     struct v4l2_buffer *buf)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	gdc_dbg("v4l2_querybuf\n");
	return v4l2_m2m_querybuf(file, ctx->m2m_ctx, buf);
}

static int gdc_check_vb2_qbuf(struct vb2_queue *q, struct v4l2_buffer *b)
{
	struct vb2_buffer *vb;
	struct vb2_plane planes[VB2_MAX_PLANES];
	int plane;
	int ret = 0;

	if (q->fileio) {
		gdc_info("file io in progress\n");
		ret = -EBUSY;
		goto q_err;
	}

	if (b->type != q->type) {
		gdc_info("buf type is invalid(%d != %d)\n",
			b->type, q->type);
		ret = -EINVAL;
		goto q_err;
	}

	if (b->index >= q->num_buffers) {
		gdc_info("buffer index out of range b_index(%d) q_num_buffers(%d)\n",
			b->index, q->num_buffers);
		ret = -EINVAL;
		goto q_err;
	}

	if (q->bufs[b->index] == NULL) {
		/* Should never happen */
		gdc_info("buffer is NULL\n");
		ret = -EINVAL;
		goto q_err;
	}

	if (b->memory != q->memory) {
		gdc_info("invalid memory type b_mem(%d) q_mem(%d)\n",
			b->memory, q->memory);
		ret = -EINVAL;
		goto q_err;
	}

	vb = q->bufs[b->index];
	if (!vb) {
		gdc_info("vb is NULL");
		ret = -EINVAL;
		goto q_err;
	}

	if (V4L2_TYPE_IS_MULTIPLANAR(b->type)) {
		/* Is memory for copying plane information present? */
		if (b->m.planes == NULL) {
			gdc_info("multi-planar buffer passed but "
				   "planes array not provided\n");
			ret = -EINVAL;
			goto q_err;
		}

		if (b->length < vb->num_planes || b->length > VB2_MAX_PLANES) {
			gdc_info("incorrect planes array length, "
				   "expected %d, got %d\n",
				   vb->num_planes, b->length);
			ret = -EINVAL;
			goto q_err;
		}
	}

	if ((b->flags & V4L2_BUF_FLAG_REQUEST_FD) &&
	    vb->state != VB2_BUF_STATE_DEQUEUED) {
		gdc_info("buffer is not in dequeued state\n");
		ret = -EINVAL;
		goto q_err;
	}

	/* for detect vb2 framework err, operate some vb2 functions */
	memset(planes, 0, sizeof(planes[0]) * vb->num_planes);
	/* Copy relevant information provided by the userspace */
	ret = call_bufop(vb->vb2_queue, fill_vb2_buffer,
			 vb, planes);
	if (ret) {
		gdc_info("vb2_fill_vb2_v4l2_buffer failed (%d)\n", ret);
		goto q_err;
	}

	for (plane = 0; plane < vb->num_planes; ++plane) {
		struct dma_buf *dbuf;

		dbuf = dma_buf_get(planes[plane].m.fd);
		if (IS_ERR_OR_NULL(dbuf)) {
			gdc_info("invalid dmabuf fd(%d) for plane %d\n",
				planes[plane].m.fd, plane);
			ret = -EINVAL;
			goto q_err;
		}

		if (planes[plane].length == 0)
			planes[plane].length = (unsigned int)dbuf->size;

		if (planes[plane].length < vb->planes[plane].min_length) {
			gdc_info("invalid dmabuf length %u for plane %d, "
				"minimum length %u\n",
				planes[plane].length, plane,
				vb->planes[plane].min_length);
			ret = -EINVAL;
			dma_buf_put(dbuf);
			goto q_err;
		}
		dma_buf_put(dbuf);
	}
q_err:
	return ret;
}

static int gdc_check_qbuf(struct file *file,
	struct v4l2_m2m_ctx *m2m_ctx, struct v4l2_buffer *buf)
{
	struct vb2_queue *vq;

	vq = v4l2_m2m_get_vq(m2m_ctx, buf->type);
	if (!V4L2_TYPE_IS_OUTPUT(vq->type) &&
	    (buf->flags & V4L2_BUF_FLAG_REQUEST_FD)) {
		gdc_info("requests cannot be used with capture buffers\n");
		return -EPERM;
	}
	return gdc_check_vb2_qbuf(vq, buf);
}

static int gdc_v4l2_qbuf(struct file *file, void *fh,
			 struct v4l2_buffer *buf)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	int ret;

	gdc_dbg("v4l2_qbuf\n");
	ret = v4l2_m2m_qbuf(file, ctx->m2m_ctx, buf);
	if (ret) {
		dev_err(ctx->gdc_dev->dev,
			"v4l2_m2m_qbuf failed ret(%d) check(%d)\n",
			ret, gdc_check_qbuf(file, ctx->m2m_ctx, buf));
	}
	return ret;
}

static int gdc_v4l2_dqbuf(struct file *file, void *fh,
			  struct v4l2_buffer *buf)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	gdc_dbg("v4l2_d_qbuf\n");
	return v4l2_m2m_dqbuf(file, ctx->m2m_ctx, buf);
}

static int gdc_power_clk_enable(struct gdc_dev *gdc)
{
	int ret;

	if (in_interrupt())
		ret = pm_runtime_get(gdc->dev);
	else
		ret = pm_runtime_get_sync(gdc->dev);

	if (ret < 0) {
		dev_err(gdc->dev,
			"%s=%d: Failed to enable local power\n", __func__, ret);
		return ret;
	}

	if (!IS_ERR(gdc->pclk)) {
		ret = clk_enable(gdc->pclk);
		if (ret) {
			dev_err(gdc->dev, "%s: Failed to enable PCLK (err %d)\n",
				__func__, ret);
			goto err_pclk;
		}
	}

	if (!IS_ERR(gdc->aclk)) {
		ret = clk_enable(gdc->aclk);
		if (ret) {
			dev_err(gdc->dev, "%s: Failed to enable ACLK (err %d)\n",
				__func__, ret);
			goto err_aclk;
		}
	}

	return 0;
err_aclk:
	if (!IS_ERR(gdc->pclk))
		clk_disable(gdc->pclk);
err_pclk:
	pm_runtime_put(gdc->dev);
	return ret;
}

static void gdc_clk_power_disable(struct gdc_dev *gdc)
{
	camerapp_hw_gdc_stop(gdc->regs_base);

	if (!IS_ERR(gdc->aclk))
		clk_disable(gdc->aclk);

	if (!IS_ERR(gdc->pclk))
		clk_disable(gdc->pclk);

	pm_runtime_put(gdc->dev);
}

static int gdc_v4l2_streamon(struct file *file, void *fh,
			     enum v4l2_buf_type type)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	struct gdc_dev *gdc = ctx->gdc_dev;
	int ret;
	gdc_dbg("v4l2_stream_on\n");

	if (gdc->stalled) {
		gdc->stalled = 0;
		gdc_dbg("stall state clear\n");
	}
	if (!V4L2_TYPE_IS_OUTPUT(type)) {
		ret = gdc_power_clk_enable(gdc);
		if (ret)
			return ret;

		gdc_dbg("gdc clk enable\n");
	}
	return v4l2_m2m_streamon(file, ctx->m2m_ctx, type);
}

static int gdc_v4l2_streamoff(struct file *file, void *fh,
			      enum v4l2_buf_type type)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(fh);
	struct gdc_dev *gdc = ctx->gdc_dev;
	gdc_dbg("v4l2_stream_off\n");
	if (!V4L2_TYPE_IS_OUTPUT(type)) {
		if (atomic_read(&gdc->m2m.in_use) == 1)
			camerapp_hw_gdc_sw_reset(gdc->regs_base);
		gdc_clk_power_disable(gdc);
	}

	return v4l2_m2m_streamoff(file, ctx->m2m_ctx, type);
}

static int gdc_v4l2_s_ctrl(struct file * file, void * priv,
			struct v4l2_control *ctrl)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(file->private_data);
	struct gdc_crop_param *crop_param = ctx->crop_param;
	int ret = 0;
	gdc_dbg("v4l2_s_ctrl = %d (%d)\n", ctrl->id, ctrl->value);

	switch (ctrl->id) {
	case V4L2_CID_CAMERAPP_GDC_GRID_CROP_START:
		crop_param->crop_start_x = (ctrl->value & 0xFFFF0000) >> 16;
		crop_param->crop_start_y = (ctrl->value & 0x0000FFFF);
		break;
	case V4L2_CID_CAMERAPP_GDC_GRID_CROP_SIZE:
		crop_param->crop_width = (ctrl->value & 0xFFFF0000) >> 16;
		crop_param->crop_height = (ctrl->value & 0x0000FFFF);
		break;
	case V4L2_CID_CAMERAPP_GDC_GRID_SENSOR_SIZE:
		crop_param->sensor_width = (ctrl->value & 0xFFFF0000) >> 16;
		crop_param->sensor_height = (ctrl->value & 0x0000FFFF);
		break;
	case V4L2_CID_CAMERAPP_SENSOR_NUM:
		crop_param->sensor_num = ctrl->value;
		gdc_dbg("sensor number = %d\n", crop_param->sensor_num);
		break;
	default:
		ret = -EINVAL;
		gdc_dbg("Err: Invalid ioctl id(%d)\n", ctrl->id);
		break;
	}

	return ret;
}

static int gdc_v4l2_s_ext_ctrls(struct file * file, void * priv,
				 struct v4l2_ext_controls * ctrls)
{
	int ret = 0;
	int i;
	struct gdc_ctx *ctx = fh_to_gdc_ctx(file->private_data);
	struct gdc_dev *gdc = ctx->gdc_dev;
	struct gdc_crop_param *crop_param;
	struct v4l2_ext_control *ext_ctrl;
	struct v4l2_control ctrl;
	u32 index;

	gdc_dbg("v4l2_s_ext_ctrl\n");

	BUG_ON(!ctx);

	for (i = 0; i < ctrls->count; i++) {
		ext_ctrl = (ctrls->controls + i);

		gdc_dbg("ctrl ID:%d\n", ext_ctrl->id);
		switch (ext_ctrl->id) {
		case V4L2_CID_CAMERAPP_GDC_GRID_CONTROL:
			{
				ret = copy_from_user(ctx->crop_param, ext_ctrl->ptr, sizeof(struct gdc_crop_param));
				index = GET_GDC_IDX(ctx->crop_param->buf_index);
				/* Check if out of crop_param array */
				if (index >= GDC_MAX_BUFS) {
					dev_err(gdc->dev, "Invalid buf_index %d, maximum is (%d)\n",
						index, GDC_MAX_BUFS);
					index = GET_GDC_IDX(ctx->crop_param->buf_index % (GDC_MAX_BUFS - 1));
				}
				/*
				 * Copy memory to keep the data for multi-buffer
				 * To support both of multi-buffer and legacy,
				 * crop_param[buffer index + 1] will be used.
				 * e.g.
				 * (multi-buffer) buffer index is 0...n and use crop_param[1...n+1]
				 * (not multi-buffer) buffer index is 0 and use crop_param[1]
				 */
				crop_param = (struct gdc_crop_param *)&ctx->crop_param[index];
				memcpy(crop_param, &ctx->crop_param, sizeof(struct gdc_crop_param));

				crop_param->is_crop_dzoom = false;

				if ((crop_param->crop_width != crop_param->sensor_width)
					|| (crop_param->crop_height != crop_param->sensor_height))
					crop_param->is_crop_dzoom = true;
			}
			break;
		default:
			ctrl.id = ext_ctrl->id;
			ctrl.value = ext_ctrl->value;

			ret = gdc_v4l2_s_ctrl(file, ctx, &ctrl);
			if (ret) {
				gdc_dbg("gdc_v4l2_s_ctrl is fail(%d)\n", ret);
				goto p_err;
			}
			break;
		}
	}

p_err:
	return ret;
}

static const struct v4l2_ioctl_ops gdc_v4l2_ioctl_ops = {
	.vidioc_querycap		= gdc_v4l2_querycap,

	.vidioc_g_fmt_vid_cap_mplane	= gdc_v4l2_g_fmt_mplane,
	.vidioc_g_fmt_vid_out_mplane	= gdc_v4l2_g_fmt_mplane,

	.vidioc_try_fmt_vid_cap_mplane	= gdc_v4l2_try_fmt_mplane,
	.vidioc_try_fmt_vid_out_mplane	= gdc_v4l2_try_fmt_mplane,

	.vidioc_s_fmt_vid_cap_mplane	= gdc_v4l2_s_fmt_mplane,
	.vidioc_s_fmt_vid_out_mplane	= gdc_v4l2_s_fmt_mplane,

	.vidioc_reqbufs			= gdc_v4l2_reqbufs,
	.vidioc_querybuf		= gdc_v4l2_querybuf,

	.vidioc_qbuf			= gdc_v4l2_qbuf,
	.vidioc_dqbuf			= gdc_v4l2_dqbuf,

	.vidioc_streamon		= gdc_v4l2_streamon,
	.vidioc_streamoff		= gdc_v4l2_streamoff,

	.vidioc_s_ctrl		= gdc_v4l2_s_ctrl,
	.vidioc_s_ext_ctrls		= gdc_v4l2_s_ext_ctrls,
};

static int gdc_ctx_stop_req(struct gdc_ctx *ctx)
{
	struct gdc_ctx *curr_ctx;
	struct gdc_dev *gdc = ctx->gdc_dev;
	int ret = 0;

	curr_ctx = v4l2_m2m_get_curr_priv(gdc->m2m.m2m_dev);
	if (!test_bit(CTX_RUN, &ctx->flags) || (curr_ctx != ctx))
		return 0;

	set_bit(CTX_ABORT, &ctx->flags);

	ret = wait_event_timeout(gdc->wait,
			!test_bit(CTX_RUN, &ctx->flags), GDC_TIMEOUT);

	/* TODO: How to handle case of timeout event */
	if (ret == 0) {
		dev_err(gdc->dev, "device failed to stop request\n");
		ret = -EBUSY;
	}

	return ret;
}

static int gdc_vb2_queue_setup(struct vb2_queue *vq,
		unsigned int *num_buffers, unsigned int *num_planes,
		unsigned int sizes[], struct device *alloc_devs[])
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vq);
	struct gdc_frame *frame;
	int i;

	gdc_dbg("gdc queue setup\n");
	frame = ctx_get_frame(ctx, vq->type);
	if (IS_ERR(frame))
		return PTR_ERR(frame);

	/* Get number of planes from format_list in driver */
	*num_planes = frame->num_planes;
	for (i = 0; i < *num_planes; i++) {
		sizes[i] = frame->bytesused[i];
		alloc_devs[i] = ctx->gdc_dev->dev;
	}

	return 0;
}

static int gdc_vb2_buf_prepare(struct vb2_buffer *vb)
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	struct gdc_frame *frame;
	int i;

	frame = ctx_get_frame(ctx, vb->vb2_queue->type);
	if (IS_ERR(frame))
		return PTR_ERR(frame);

	if (!V4L2_TYPE_IS_OUTPUT(vb->vb2_queue->type)) {
		for (i = 0; i < frame->gdc_fmt->num_planes; i++)
			vb2_set_plane_payload(vb, i, frame->bytesused[i]);
	}

	return 0;
}

static void gdc_vb2_buf_finish(struct vb2_buffer *vb)
{
	/* No operation */
}

static void gdc_vb2_buf_queue(struct vb2_buffer *vb)
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	struct vb2_v4l2_buffer *v4l2_buf = to_vb2_v4l2_buffer(vb);

	gdc_dbg("gdc buf_queue\n");

	if (ctx->m2m_ctx)
		v4l2_m2m_buf_queue(ctx->m2m_ctx, v4l2_buf);
}

static void gdc_vb2_buf_cleanup(struct vb2_buffer *vb)
{
	/* No operation */
}

static void gdc_vb2_lock(struct vb2_queue *vq)
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vq);
	mutex_lock(&ctx->gdc_dev->lock);
}

static void gdc_vb2_unlock(struct vb2_queue *vq)
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vq);
	mutex_unlock(&ctx->gdc_dev->lock);
}

static void gdc_cleanup_queue(struct gdc_ctx *ctx)
{
	struct vb2_v4l2_buffer *src_vb, *dst_vb;

	while (v4l2_m2m_num_src_bufs_ready(ctx->m2m_ctx) > 0) {
		src_vb = v4l2_m2m_src_buf_remove(ctx->m2m_ctx);
		v4l2_m2m_buf_done(src_vb, VB2_BUF_STATE_ERROR);
		gdc_dbg("src_index(%d)\n", src_vb->vb2_buf.index);
	}

	while (v4l2_m2m_num_dst_bufs_ready(ctx->m2m_ctx) > 0) {
		dst_vb = v4l2_m2m_dst_buf_remove(ctx->m2m_ctx);
		v4l2_m2m_buf_done(dst_vb, VB2_BUF_STATE_ERROR);
		gdc_dbg("dst_index(%d)\n", dst_vb->vb2_buf.index);
	}
}

static int gdc_vb2_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vq);
	set_bit(CTX_STREAMING, &ctx->flags);

	return 0;
}

static void gdc_vb2_stop_streaming(struct vb2_queue *vq)
{
	struct gdc_ctx *ctx = vb2_get_drv_priv(vq);
	int ret;

	ret = gdc_ctx_stop_req(ctx);
	if (ret < 0)
		dev_err(ctx->gdc_dev->dev, "wait timeout\n");

	clear_bit(CTX_STREAMING, &ctx->flags);

	/* release all queued buffers in multi-buffer scenario*/
	gdc_cleanup_queue(ctx);
}

static struct vb2_ops gdc_vb2_ops = {
	.queue_setup		= gdc_vb2_queue_setup,
	.buf_prepare		= gdc_vb2_buf_prepare,
	.buf_finish		= gdc_vb2_buf_finish,
	.buf_queue		= gdc_vb2_buf_queue,
	.buf_cleanup		= gdc_vb2_buf_cleanup,
	.wait_finish		= gdc_vb2_lock,
	.wait_prepare		= gdc_vb2_unlock,
	.start_streaming	= gdc_vb2_start_streaming,
	.stop_streaming		= gdc_vb2_stop_streaming,
};

static int queue_init(void *priv, struct vb2_queue *src_vq,
		      struct vb2_queue *dst_vq)
{
	struct gdc_ctx *ctx = priv;
	int ret;

	memset(src_vq, 0, sizeof(*src_vq));
	src_vq->type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	src_vq->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	src_vq->ops = &gdc_vb2_ops;
	src_vq->mem_ops = &vb2_dma_sg_memops;
	src_vq->drv_priv = ctx;
	src_vq->buf_struct_size = sizeof(struct vb2_gdc_buffer);
	src_vq->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY;

	ret = vb2_queue_init(src_vq);
	if (ret)
		return ret;

	memset(dst_vq, 0, sizeof(*dst_vq));
	dst_vq->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	dst_vq->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	dst_vq->ops = &gdc_vb2_ops;
	dst_vq->mem_ops = &vb2_dma_sg_memops;
	dst_vq->drv_priv = ctx;
	dst_vq->buf_struct_size = sizeof(struct vb2_gdc_buffer);
	dst_vq->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY;

	return vb2_queue_init(dst_vq);
}

static int gdc_open(struct file *file)
{
	struct gdc_dev *gdc = video_drvdata(file);
	struct gdc_ctx *ctx;
	int ret = 0;

	ctx = vmalloc(sizeof(struct gdc_ctx));

	if (!ctx) {
		dev_err(gdc->dev, "no memory for open context\n");
		return -ENOMEM;
	}

	atomic_inc(&gdc->m2m.in_use);

	INIT_LIST_HEAD(&ctx->node);
	ctx->gdc_dev = gdc;

	v4l2_fh_init(&ctx->fh, gdc->m2m.vfd);
	file->private_data = &ctx->fh;
	v4l2_fh_add(&ctx->fh);

	/* Default color format */
	ctx->s_frame.gdc_fmt = &gdc_formats[0];
	ctx->d_frame.gdc_fmt = &gdc_formats[0];

	if (!IS_ERR(gdc->pclk)) {
		ret = clk_prepare(gdc->pclk);
		if (ret) {
			dev_err(gdc->dev, "%s: failed to prepare PCLK(err %d)\n",
					__func__, ret);
			goto err_pclk_prepare;
		}
	}

	if (!IS_ERR(gdc->aclk)) {
		ret = clk_prepare(gdc->aclk);
		if (ret) {
			dev_err(gdc->dev, "%s: failed to prepare ACLK(err %d)\n",
					__func__, ret);
			goto err_aclk_prepare;
		}
	}

	/* Setup the device context for mem2mem mode. */
	ctx->m2m_ctx = v4l2_m2m_ctx_init(gdc->m2m.m2m_dev, ctx, queue_init);
	if (IS_ERR(ctx->m2m_ctx)) {
		ret = -EINVAL;
		goto err_ctx;
	}

	gdc_dbg("gdc open = %d\n", ret);
	return 0;

err_ctx:
	if (!IS_ERR(gdc->aclk))
		clk_unprepare(gdc->aclk);
err_aclk_prepare:
	if (!IS_ERR(gdc->pclk))
		clk_unprepare(gdc->pclk);
err_pclk_prepare:
	v4l2_fh_del(&ctx->fh);
	v4l2_fh_exit(&ctx->fh);
	atomic_dec(&gdc->m2m.in_use);
	vfree(ctx);

	return ret;
}

static void gdc_job_finish(struct gdc_dev *gdc, struct gdc_ctx *ctx)
{
	unsigned long flags;
	struct vb2_v4l2_buffer *src_vb, *dst_vb;

	spin_lock_irqsave(&gdc->slock, flags);

	ctx = v4l2_m2m_get_curr_priv(gdc->m2m.m2m_dev);
	if (!ctx || !ctx->m2m_ctx) {
		dev_err(gdc->dev, "current ctx is NULL\n");
		spin_unlock_irqrestore(&gdc->slock, flags);
		return;

	}
	clear_bit(CTX_RUN, &ctx->flags);

	src_vb = v4l2_m2m_src_buf_remove(ctx->m2m_ctx);
	dst_vb = v4l2_m2m_dst_buf_remove(ctx->m2m_ctx);

	BUG_ON(!src_vb || !dst_vb);

	v4l2_m2m_buf_done(src_vb, VB2_BUF_STATE_ERROR);
	v4l2_m2m_buf_done(dst_vb, VB2_BUF_STATE_ERROR);

	v4l2_m2m_job_finish(gdc->m2m.m2m_dev, ctx->m2m_ctx);

	spin_unlock_irqrestore(&gdc->slock, flags);
}

static int gdc_release(struct file *file)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(file->private_data);
	struct gdc_dev *gdc = ctx->gdc_dev;

	gdc_dbg("refcnt= %d", atomic_read(&gdc->m2m.in_use));

	atomic_dec(&gdc->m2m.in_use);

	gdc_dbg("gdc close\n");

	if (!atomic_read(&gdc->m2m.in_use) && test_bit(DEV_RUN, &gdc->state)) {
		dev_err(gdc->dev, "%s, gdc is still running\n", __func__);
		gdc_suspend(gdc->dev);
	}

	v4l2_m2m_ctx_release(ctx->m2m_ctx);
	if (!IS_ERR(gdc->aclk))
		clk_unprepare(gdc->aclk);
	if (!IS_ERR(gdc->pclk))
		clk_unprepare(gdc->pclk);
	v4l2_fh_del(&ctx->fh);
	v4l2_fh_exit(&ctx->fh);
	vfree(ctx);

	return 0;
}

static unsigned int gdc_poll(struct file *file,
			     struct poll_table_struct *wait)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(file->private_data);

	return v4l2_m2m_poll(file, ctx->m2m_ctx, wait);
}

static int gdc_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct gdc_ctx *ctx = fh_to_gdc_ctx(file->private_data);

	return v4l2_m2m_mmap(file, ctx->m2m_ctx, vma);
}

static const struct v4l2_file_operations gdc_v4l2_fops = {
	.owner		= THIS_MODULE,
	.open		= gdc_open,
	.release	= gdc_release,
	.poll		= gdc_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= gdc_mmap,
};

static void gdc_watchdog(struct timer_list *t)
{
	struct gdc_wdt *wdt = from_timer(wdt, t, timer);
	struct gdc_dev *gdc = container_of(wdt, typeof(*gdc), wdt);
	struct gdc_crop_param *crop_param;
	struct gdc_ctx *ctx;
	unsigned long flags;

	if (!test_bit(DEV_RUN, &gdc->state)) {
		gdc_info("GDC is not running\n");
		return;
	}

	spin_lock_irqsave(&gdc->ctxlist_lock, flags);
	ctx = gdc->current_ctx;
	if (!ctx) {
		gdc_info("ctx is empty\n");
		spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);
		return;
	}

	crop_param = &ctx->crop_param[ctx->cur_index];

	if (atomic_read(&gdc->wdt.cnt) >= GDC_WDT_CNT) {
		gdc_dbg("final timeout\n");
		is_debug_s2d(true, "GDC watchdog s2d");
		if (gdc->has_votf_mfc == 1 && crop_param->votf_en) {
			gdc->stalled = 1;
			gdc_votfitf_set_flush(gdc);
			if (votfitf_wrapper_reset(gdc->votf_base))
				dev_err(gdc->dev, "gdc votf wrapper reset fail\n");
		}
		if (camerapp_hw_gdc_sw_reset(gdc->regs_base))
			dev_err(gdc->dev, "gdc sw reset fail\n");

		atomic_set(&gdc->wdt.cnt, 0);
		clear_bit(DEV_RUN, &gdc->state);
		gdc->current_ctx = NULL;
		gdc->votf_ctx = NULL;
		spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);
		gdc_job_finish(gdc, ctx);
		return;
	}

	if (gdc->has_votf_mfc == 1 && crop_param->votf_en) {
		if (atomic_read(&gdc->wdt.cnt) >= 1) {
			if (!votfitf_check_votf_ring(gdc->mfc_votf_base, C2SERV)) {
				gdc_dbg("mfc off state\n");
				gdc->stalled = 1;
				/* Do not votf_flush when MFC is turned off */
				if (votfitf_wrapper_reset(gdc->votf_base))
					dev_err(gdc->dev, "gdc votf wrapper reset fail\n");

				if (camerapp_hw_gdc_sw_reset(gdc->regs_base))
					dev_err(gdc->dev, "gdc sw reset fail\n");

				atomic_set(&gdc->wdt.cnt, 0);
				clear_bit(DEV_RUN, &gdc->state);
				gdc->current_ctx = NULL;
				gdc->votf_ctx = NULL;
				spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);
				gdc_job_finish(gdc, ctx);
				return;
			}
			gdc_dbg("Waiting for MFC operation to end\n");
		}
	}
	spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);

	if (test_bit(DEV_RUN, &gdc->state)) {
		if (!atomic_read(&gdc->wdt.cnt))
			camerapp_gdc_sfr_dump(gdc->regs_base);
		atomic_inc(&gdc->wdt.cnt);
		dev_err(gdc->dev, "gdc is still running\n");
		mod_timer(&gdc->wdt.timer, jiffies + GDC_TIMEOUT);
	} else {
		gdc_dbg("gdc finished job\n");
	}
}

int gdc_votf_create_link(void __iomem *base_addr, struct gdc_dev *gdc)
{
	int ret = 0;

	votfitf_votf_create_ring(gdc->votf_base,
		gdc->votf_src_dest[GDC_SRC_ADDR] >> 16, C2SERV);
	votfitf_votf_set_sel_reg(gdc->votf_base, 0x1, 0x1);

	ret = gdc_votf_set_service_config(TWS, gdc);
	if (ret < 0) {
		gdc_dbg("create link error (ret:%d)\n", ret);
		return ret;
	}
	camerapp_hw_gdc_votf_enable(base_addr, TWS);

	return ret;
}

static int gdc_run_next_job(struct gdc_dev *gdc)
{
	unsigned long flags;
	struct gdc_ctx *ctx;
	struct gdc_crop_param *crop_param;
	int ret;

	spin_lock_irqsave(&gdc->ctxlist_lock, flags);

	if (gdc->current_ctx || list_empty(&gdc->context_list)) {
		/* a job is currently being processed or no job is to run */
		spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);
		return 0;
	}

	ctx = list_first_entry(&gdc->context_list, struct gdc_ctx, node);

	list_del_init(&ctx->node);

	gdc->current_ctx = ctx;
	crop_param = &ctx->crop_param[ctx->cur_index];

	if (gdc->stalled) {
		dev_err(gdc->dev, "%s, gdc hw stalled!!\n", __func__);
		gdc->current_ctx = NULL;
		gdc->votf_ctx = NULL;
		spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);
		gdc_job_finish(gdc, ctx);
		return 0;
	}

	spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);

	/*
	 * gdc_run_next_job() must not reenter while gdc->state is DEV_RUN.
	 * DEV_RUN is cleared when an operation is finished.
	 */
	BUG_ON(test_bit(DEV_RUN, &gdc->state));

	gdc_dbg("gdc hw setting\n");
	if (gdc->has_votf_mfc == 1 && crop_param->votf_en)
		votfitf_wrapper_reset(gdc->votf_base);

	camerapp_hw_gdc_sw_reset(gdc->regs_base);
	gdc_dbg("gdc sw reset\n");

	if (gdc->has_votf_mfc == 1 && crop_param->votf_en) {
		ret = gdc_votf_create_link(gdc->regs_base, gdc);
		if (ret < 0) {
			dev_err(gdc->dev,
				"gdc votf create link fail (ret:%d)\n", ret);
			return ret;
		}
	}

	camerapp_hw_gdc_update_param(gdc->regs_base, gdc);
	gdc_dbg("gdc param update done\n");

	set_bit(DEV_RUN, &gdc->state);
	set_bit(CTX_RUN, &ctx->flags);
	mod_timer(&gdc->wdt.timer, jiffies + GDC_TIMEOUT);

	camerapp_hw_gdc_start(gdc->regs_base);

	return 0;
}

static int gdc_add_context_and_run(struct gdc_dev *gdc, struct gdc_ctx *ctx)
{
	unsigned long flags;

	spin_lock_irqsave(&gdc->ctxlist_lock, flags);
	list_add_tail(&ctx->node, &gdc->context_list);
	spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);

	return gdc_run_next_job(gdc);
}

static irqreturn_t gdc_irq_handler(int irq, void *priv)
{
	struct gdc_dev *gdc = priv;
	struct gdc_ctx *ctx;
	struct vb2_v4l2_buffer *src_vb, *dst_vb;
	u32 irq_status;

	spin_lock(&gdc->slock);

	irq_status = camerapp_hw_gdc_get_intr_status_and_clear(gdc->regs_base);
	if (gdc->stalled) {
		gdc_dbg("stalled intr = %x\n", irq_status);
		spin_unlock(&gdc->slock);
		return IRQ_HANDLED;
	}

	/*
	 * ok to access gdc->current_ctx withot ctxlist_lock held
	 * because it is not modified until gdc_run_next_job() is called.
	 */
	ctx = gdc->current_ctx;
	BUG_ON(!ctx);

	if (irq_status & camerapp_hw_gdc_get_int_frame_start()) {
		if (__gdc_measure_hw_latency) {
			struct vb2_v4l2_buffer *vb =
				v4l2_m2m_next_dst_buf(ctx->m2m_ctx);
			struct v4l2_m2m_buffer *mb =
				container_of(vb, typeof(*mb), vb);
			struct vb2_gdc_buffer *svb =
				container_of(mb, typeof(*svb), mb);
			svb->ktime = ktime_get();
		}
		gdc_dbg("gdc frame start (0x%x)\n", irq_status);
		if (__gdc_sfr_dump) {
			camerapp_gdc_sfr_dump(gdc->regs_base);
			__gdc_sfr_dump = 0;
		}
	}

	if (irq_status & camerapp_hw_gdc_get_int_frame_end()) {
		clear_bit(DEV_RUN, &gdc->state);
		del_timer(&gdc->wdt.timer);
		atomic_set(&gdc->wdt.cnt, 0);

		clear_bit(CTX_RUN, &ctx->flags);

		BUG_ON(ctx != v4l2_m2m_get_curr_priv(gdc->m2m.m2m_dev));

		src_vb = v4l2_m2m_src_buf_remove(ctx->m2m_ctx);
		dst_vb = v4l2_m2m_dst_buf_remove(ctx->m2m_ctx);

		BUG_ON(!src_vb || !dst_vb);

		if (__gdc_measure_hw_latency) {
			struct v4l2_m2m_buffer *mb =
				container_of(dst_vb, typeof(*mb), vb);
			struct vb2_gdc_buffer *svb =
				container_of(mb, typeof(*svb), mb);

			dst_vb->vb2_buf.timestamp =
				(__u32)ktime_us_delta(ktime_get(), svb->ktime);
			gdc_dbg("gdc_hw_latency %lld us\n",
					dst_vb->vb2_buf.timestamp);
		}
		gdc_dbg("gdc frame end (0x%x)\n", irq_status);

		v4l2_m2m_buf_done(src_vb, VB2_BUF_STATE_DONE);
		v4l2_m2m_buf_done(dst_vb, VB2_BUF_STATE_DONE);

		/* Wake up from CTX_ABORT state */
		clear_bit(CTX_ABORT, &ctx->flags);

		spin_lock(&gdc->ctxlist_lock);
		gdc->current_ctx = NULL;
		gdc->votf_ctx = NULL;
		spin_unlock(&gdc->ctxlist_lock);

		v4l2_m2m_job_finish(gdc->m2m.m2m_dev, ctx->m2m_ctx);

		gdc_resume(gdc->dev);
		wake_up(&gdc->wait);
	}

	spin_unlock(&gdc->slock);

	return IRQ_HANDLED;
}

static int gdc_get_bufaddr(struct gdc_dev *gdc, struct gdc_ctx *ctx,
		struct vb2_buffer *vb2buf, struct gdc_frame *frame)
{
	unsigned int pixsize, bytesize;
	unsigned int w = frame->width;
	unsigned int h = frame->height;

	pixsize = frame->width * frame->height;
	bytesize = (pixsize * frame->gdc_fmt->bitperpixel[0]) >> 3;

	frame->addr.y = gdc_get_dma_address(vb2buf, 0);
	if (!frame->addr.y)
		return -EINVAL;

	frame->addr.cb = 0;
	frame->addr.cr = 0;
	frame->addr.y_2bit = 0;
	frame->addr.cbcr_2bit = 0;
	frame->addr.cbsize = 0;
	frame->addr.crsize = 0;
	frame->addr.ysize_2bit = 0;
	frame->addr.cbcrsize_2bit = 0;

	switch (frame->gdc_fmt->num_comp) {
	case 1: /* rgb, yuyv */
		frame->addr.ysize = bytesize;
		break;
	case 2:
		if (frame->gdc_fmt->num_planes == 1) {
			/* V4L2_PIX_FMT_NV12, V4L2_PIX_FMT_NV21,  V4L2_PIX_FMT_NV61, V4L2_PIX_FMT_NV16 */
			frame->addr.cb = frame->addr.y + pixsize;
			frame->addr.ysize = pixsize;
			frame->addr.cbsize = bytesize - pixsize;
		} else if (frame->gdc_fmt->num_planes == 2) {
			/* V4L2_PIX_FMT_NV21M, V4L2_PIX_FMT_NV12M */
			/* V4L2_PIX_FMT_NV61M, V4L2_PIX_FMT_NV16M */
			/* V4L2_PIX_FMT_NV12M_P010, V4L2_PIX_FMT_NV16M_P210 */
			/* V4L2_PIX_FMT_NV21M_P010, V4L2_PIX_FMT_NV61M_P210 */
			frame->addr.cb = gdc_get_dma_address(vb2buf, 1);
			if (!frame->addr.cb)
				return -EINVAL;
			/* SBWC format */
			if (frame->extra == COMP) {
				/*
				 * When SBWC is on, Buffer is consist of payload(before) + header(after).
				 * Header base address is payload base address + payload memory size.
				 */
				if (frame->pixel_size == CAMERAPP_PIXEL_SIZE_8BIT) {
					frame->addr.y_2bit = frame->addr.y + SBWC_8B_Y_SIZE(w, h);
					frame->addr.cbcr_2bit = frame->addr.cb + SBWC_8B_CBCR_SIZE(w, h);
				} else if (frame->pixel_size == CAMERAPP_PIXEL_SIZE_10BIT) {
					frame->addr.y_2bit = frame->addr.y + SBWC_10B_Y_SIZE(w, h);
					frame->addr.cbcr_2bit = frame->addr.cb + SBWC_10B_CBCR_SIZE(w, h);
				} else {
					dev_err(gdc->dev, "Please check frame->pixel_size for SBWC\n");
				}
			}
			/* 8+2 format */
			if ((frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_S10B)
					|| (frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_S10B)) {
				frame->addr.ysize_2bit = NV16M_Y_SIZE(w, h);
				frame->addr.y_2bit = frame->addr.y + NV16M_Y_SIZE(w, h);
				frame->addr.cbcrsize_2bit = NV16M_CBCR_SIZE(w, h);
				frame->addr.cbcr_2bit = frame->addr.cb + NV16M_CBCR_SIZE(w,h);
			} else if ((frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_S10B)
					|| (frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_S10B)) {
				frame->addr.ysize_2bit = NV12M_Y_SIZE(w, h);
				frame->addr.y_2bit = frame->addr.y + NV12M_Y_SIZE(w, h);
				frame->addr.cbcrsize_2bit = NV12M_CBCR_SIZE(w, h);
				frame->addr.cbcr_2bit = frame->addr.cb + NV12M_CBCR_SIZE(w,h);
			}
		} else {
			dev_err(gdc->dev, "Please check frame->gdc_fmt->pixelformat\n");
		}
		break;
	case 3:
		if (frame->gdc_fmt->num_planes == 1) {
			if (gdc_fmt_is_ayv12(frame->gdc_fmt->pixelformat)) {
				unsigned int c_span;
				c_span = ALIGN(frame->width >> 1, 16);
				frame->addr.ysize = pixsize;
				frame->addr.cbsize = c_span * (frame->height >> 1);
				frame->addr.crsize = frame->addr.cbsize;
				frame->addr.cb = frame->addr.y + pixsize;
				frame->addr.cr = frame->addr.cb + frame->addr.cbsize;
			} else if (frame->gdc_fmt->pixelformat ==
					V4L2_PIX_FMT_YUV420N) {
				unsigned int w = frame->width;
				unsigned int h = frame->height;
				frame->addr.ysize = YUV420N_Y_SIZE(w, h);
				frame->addr.cbsize = YUV420N_CB_SIZE(w, h);
				frame->addr.crsize = YUV420N_CR_SIZE(w, h);
				frame->addr.cb =
					YUV420N_CB_BASE(frame->addr.y, w, h);
				frame->addr.cr =
					YUV420N_CR_BASE(frame->addr.y, w, h);
			} else {
				frame->addr.ysize = pixsize;
				frame->addr.cbsize = (bytesize - pixsize) / 2;
				frame->addr.crsize = frame->addr.cbsize;
				frame->addr.cb = frame->addr.y + pixsize;
				frame->addr.cr = frame->addr.cb + frame->addr.cbsize;
			}
		} else if (frame->gdc_fmt->num_planes == 3) {
			frame->addr.cb = gdc_get_dma_address(vb2buf, 1);
			if (!frame->addr.cb)
				return -EINVAL;

			frame->addr.cr = gdc_get_dma_address(vb2buf, 2);
			if (!frame->addr.cr)
				return -EINVAL;
			frame->addr.ysize =
				pixsize * frame->gdc_fmt->bitperpixel[0] >> 3;
			frame->addr.cbsize =
				pixsize * frame->gdc_fmt->bitperpixel[1] >> 3;
			frame->addr.crsize =
				pixsize * frame->gdc_fmt->bitperpixel[2] >> 3;
		}
		break;
	default:
		break;
	}

	if (frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_YVU420 || frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_YVU420M) {
		u32 t_cb = frame->addr.cb;
		frame->addr.cb = frame->addr.cr;
		frame->addr.cr = t_cb;
	}

#ifdef ENABLE_PRINT_GDC_BUFADDR
	dev_info(gdc->dev, "y addr %pa y size %#x\n", &frame->addr.y, frame->addr.ysize);
	dev_info(gdc->dev, "cb addr %pa cb size %#x\n", &frame->addr.cb, frame->addr.cbsize);
	dev_info(gdc->dev, "cr addr %pa cr size %#x\n", &frame->addr.cr, frame->addr.crsize);
#endif

	return 0;
}

static void gdc_fill_curr_frame(struct gdc_dev *gdc, struct gdc_ctx *ctx)
{
	struct gdc_frame *s_frame, *d_frame;
	struct gdc_metadata *s_meta, *d_meta;
	struct vb2_buffer *src_vb = (struct vb2_buffer *)v4l2_m2m_next_src_buf(ctx->m2m_ctx);
	struct vb2_buffer *dst_vb = (struct vb2_buffer *)v4l2_m2m_next_dst_buf(ctx->m2m_ctx);
	u32 buf_index = GET_GDC_IDX(ctx->crop_param[GET_GDC_IDX(src_vb->index)].buf_index);
	struct gdc_crop_param *crop_param = &ctx->crop_param[buf_index];

	s_frame = &ctx->s_frame;
	d_frame = &ctx->d_frame;
	/* Meta plane exists */
	if (s_frame->num_planes > s_frame->gdc_fmt->num_planes) {
		s_meta = (struct gdc_metadata *)gdc_get_kvaddr(src_vb, s_frame->num_planes - 1);
		s_frame->width = (s_meta->full_width != 0) ? s_meta->full_width : s_frame->width;
		s_frame->height = (s_meta->full_height != 0) ? s_meta->full_height : s_frame->height;
		s_frame->gdc_fmt = (s_meta->pixel_format != 0) ?
			gdc_find_format(ctx->gdc_dev, s_meta->pixel_format, NULL) : s_frame->gdc_fmt;
	}
	if (d_frame->num_planes > d_frame->gdc_fmt->num_planes) {
		d_meta = (struct gdc_metadata *)gdc_get_kvaddr(dst_vb, d_frame->num_planes - 1);
		d_frame->width = (d_meta->full_width != 0) ? d_meta->full_width : d_frame->width;
		d_frame->height = (d_meta->full_height != 0) ? d_meta->full_height : d_frame->height;
		d_frame->gdc_fmt = (d_meta->pixel_format != 0) ?
			gdc_find_format(ctx->gdc_dev, d_meta->pixel_format, NULL) : d_frame->gdc_fmt;
	}

	/* Check input crop boundary */
	ctx->cur_index = buf_index;
	if (crop_param->crop_start_x + crop_param->crop_width > s_frame->width
		|| crop_param->crop_start_y + crop_param->crop_height > s_frame->height) {
		dev_err(gdc->dev, "Invalid input crop size %d,%d %dx%d. Input size %dx%d\n",
				crop_param->crop_start_x, crop_param->crop_start_y,
				crop_param->crop_width, crop_param->crop_height,
				s_frame->width, s_frame->height);
		crop_param->crop_start_x = 0;
		crop_param->crop_start_y = 0;
		crop_param->crop_width = s_frame->width;
		crop_param->crop_height = s_frame->height;
	}
	gdc_get_bufaddr(gdc, ctx, src_vb, s_frame);
	gdc_get_bufaddr(gdc, ctx, dst_vb, d_frame);

	gdc_dbg("gdc_src : format = %d, w = %d, h = %d\n", s_frame->gdc_fmt->cfg_val, s_frame->width, s_frame->height);
	gdc_dbg("gdc_dst : format = %d, w = %d, h = %d\n", d_frame->gdc_fmt->cfg_val, d_frame->width, d_frame->height);
	gdc_dbg("gdc_crop_param[%d] : crop_x:%d, crop_y:%d crop_width:%d crop_height:%d crop_votf:%d grid:%d bypass:%d\n",
		buf_index,
		crop_param->crop_start_x, crop_param->crop_start_y,
		crop_param->crop_width, crop_param->crop_height,
		crop_param->votf_en, crop_param->is_grid_mode, crop_param->is_bypass_mode);
}

static unsigned long gdc_get_dst_inode_num(struct gdc_ctx *ctx)
{
	struct vb2_buffer *dst_vb;
	unsigned long inode_num;

	dst_vb = (struct vb2_buffer *)v4l2_m2m_next_dst_buf(ctx->m2m_ctx);
	inode_num = file_inode(dst_vb->planes[0].dbuf->file)->i_ino;

	gdc_dbg("fd(%d) inode_num(%ld)\n", dst_vb->planes[0].m.fd, inode_num);

	return inode_num;
}

static int gdc_check_received_buffer(struct gdc_ctx *ctx, unsigned long mfc_ino)
{
	struct vb2_buffer *vb2_buf;
	struct v4l2_m2m_buffer *v4l2_buf;
	unsigned long inode_num;
	int ret = GDC_NO_INO, idx = 0;

	v4l2_m2m_for_each_dst_buf(ctx->m2m_ctx, v4l2_buf) {
		vb2_buf = &v4l2_buf->vb.vb2_buf;
		inode_num = file_inode(vb2_buf->planes[0].dbuf->file)->i_ino;
		gdc_dbg("buffer[%d] gdc_inode(%ld), mfc_inode(%ld)\n", idx, inode_num, mfc_ino);

		if (inode_num == mfc_ino) {
			if (idx) {
				ret = GDC_DROP_INO;
				gdc_info("MFC drop (%d) frames\n", idx);
			} else
				ret = GDC_GOOD_INO;

			break;
		}
		idx++;
	}

	return ret;
}

static int gdc_get_votf_scenario(struct gdc_ctx *ctx)
{
	struct vb2_buffer *src_vb = (struct vb2_buffer *)v4l2_m2m_next_src_buf(ctx->m2m_ctx);
	u32 buf_index = GET_GDC_IDX(ctx->crop_param[GET_GDC_IDX(src_vb->index)].buf_index);
	struct gdc_crop_param *crop_param = &ctx->crop_param[buf_index];

	gdc_dbg("votf_en(%d)\n", crop_param->votf_en);

	return crop_param->votf_en;
}

static void gdc_m2m_device_run(void *priv)
{
	struct gdc_ctx *ctx = priv;
	struct gdc_dev *gdc = ctx->gdc_dev;
	unsigned long dst_i_ino;
	unsigned long flags;

	gdc_dbg("gdc m2m device run\n");

	if (test_bit(DEV_SUSPEND, &gdc->state)) {
		dev_err(gdc->dev, "GDC is in suspend state\n");
		return;
	}

	if (test_bit(CTX_ABORT, &ctx->flags)) {
		dev_err(gdc->dev, "aborted GDC device run\n");
		return;
	}

	if (gdc_get_votf_scenario(ctx)) {
		spin_lock_irqsave(&gdc->ctxlist_lock, flags);
		gdc->votf_ctx = ctx;
		spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);

		if (!gdc->gdc_ready_frame_cb)
			atomic_set(&gdc->wait_mfc, 1);
		else {
			dst_i_ino = gdc_get_dst_inode_num(ctx);
			if (gdc->gdc_ready_frame_cb(dst_i_ino) < 0) {
				gdc_info("MFC buffer is abnormal(i_ino:%ld)\n", dst_i_ino);
				gdc_job_finish(gdc, ctx);
			}
			gdc_dbg("Waiting for MFC call(i_ino:%d)\n", dst_i_ino);
		}
		return;
	}

	gdc_fill_curr_frame(gdc, ctx);
	gdc_add_context_and_run(gdc, ctx);
}

int gdc_device_run(unsigned long i_ino)
{
	unsigned long flags;
	struct gdc_ctx *ctx;
	struct gdc_dev *gdc =
		(struct gdc_dev *)dev_get_drvdata(gdc_device);
	int ret;

	if (!gdc) {
		gdc_info("no gdc_dev\n");
		return -ENOMEM;
	}

	if (!gdc->votf_ctx) {
		gdc_info("no votf_ctx\n");
		return -ENOMEM;
	}
	if (!i_ino) {
		gdc_info("no mfc i_ino(%ld)\n ", i_ino);
		return -EINVAL;
	}
	gdc_dbg("received inode : (%ld)\n", i_ino);

	spin_lock_irqsave(&gdc->ctxlist_lock, flags);
	ctx = gdc->votf_ctx;
	spin_unlock_irqrestore(&gdc->ctxlist_lock, flags);

	ret = gdc_check_received_buffer(ctx, i_ino);
	if (ret == GDC_GOOD_INO) {
		gdc_fill_curr_frame(gdc, ctx);
		gdc_add_context_and_run(gdc, ctx);
	} else if (ret == GDC_DROP_INO)
		gdc_job_finish(gdc, ctx);
	/* else == GDC_NO_FD */
	return ret;
}
EXPORT_SYMBOL(gdc_device_run);

int gdc_register_ready_frame_cb(int (*gdc_ready_frame_cb)(unsigned long i_ino))
{
	unsigned long dst_i_ino;
	struct gdc_dev *gdc =
		(struct gdc_dev *)dev_get_drvdata(gdc_device);

	if (!gdc) {
		gdc_info("no gdc_dev\n");
		return -ENOMEM;
	}

	if (!gdc_ready_frame_cb) {
		gdc_info("no gdc_ready_frame_cb\n");
		return -ENOMEM;
	}

	gdc->gdc_ready_frame_cb = gdc_ready_frame_cb;

	gdc_dbg("done\n");

	if (atomic_read(&gdc->wait_mfc)) {
		/* first frame only */
		atomic_set(&gdc->wait_mfc, 0);
		dst_i_ino = gdc_get_dst_inode_num(gdc->votf_ctx);
		if (gdc->gdc_ready_frame_cb(dst_i_ino) < 0) {
			gdc_info("MFC buffer is abnormal(i_ino:%ld)\n", dst_i_ino);
			gdc_job_finish(gdc, gdc->votf_ctx);
		}
	}

	return 0;
}
EXPORT_SYMBOL(gdc_register_ready_frame_cb);

void gdc_unregister_ready_frame_cb(void)
{
	struct gdc_dev *gdc =
		(struct gdc_dev *)dev_get_drvdata(gdc_device);

	if (!gdc) {
		gdc_info("no gdc_dev\n");
		return;
	}

	gdc->gdc_ready_frame_cb = NULL;
	gdc_dbg("done\n");
}
EXPORT_SYMBOL(gdc_unregister_ready_frame_cb);

static void gdc_m2m_job_abort(void *priv)
{
	struct gdc_ctx *ctx = priv;
	int ret;

	ret = gdc_ctx_stop_req(ctx);
	if (ret < 0)
		dev_err(ctx->gdc_dev->dev, "wait timeout\n");
}

static struct v4l2_m2m_ops gdc_m2m_ops = {
	.device_run	= gdc_m2m_device_run,
	.job_abort	= gdc_m2m_job_abort,
};

static void gdc_unregister_m2m_device(struct gdc_dev *gdc)
{
	v4l2_m2m_release(gdc->m2m.m2m_dev);
	video_device_release(gdc->m2m.vfd);
	v4l2_device_unregister(&gdc->m2m.v4l2_dev);
}

static int gdc_register_m2m_device(struct gdc_dev *gdc, int dev_id)
{
	struct v4l2_device *v4l2_dev;
	struct device *dev;
	struct video_device *vfd;
	int ret = 0;

	dev = gdc->dev;
	v4l2_dev = &gdc->m2m.v4l2_dev;

	scnprintf(v4l2_dev->name, sizeof(v4l2_dev->name), "%s.m2m",
			GDC_MODULE_NAME);

	ret = v4l2_device_register(dev, v4l2_dev);
	if (ret) {
		dev_err(gdc->dev, "failed to register v4l2 device\n");
		return ret;
	}

	vfd = video_device_alloc();
	if (!vfd) {
		dev_err(gdc->dev, "failed to allocate video device\n");
		goto err_v4l2_dev;
	}

	vfd->fops	= &gdc_v4l2_fops;
	vfd->ioctl_ops	= &gdc_v4l2_ioctl_ops;
	vfd->release	= video_device_release;
	vfd->lock	= &gdc->lock;
	vfd->vfl_dir	= VFL_DIR_M2M;
	vfd->v4l2_dev	= v4l2_dev;
	vfd->device_caps = V4L2_CAP_VIDEO_M2M_MPLANE;
	scnprintf(vfd->name, sizeof(vfd->name), "%s:m2m", GDC_MODULE_NAME);

	video_set_drvdata(vfd, gdc);

	gdc->m2m.vfd = vfd;
	gdc->m2m.m2m_dev = v4l2_m2m_init(&gdc_m2m_ops);
	if (IS_ERR(gdc->m2m.m2m_dev)) {
		dev_err(gdc->dev, "failed to initialize v4l2-m2m device\n");
		ret = PTR_ERR(gdc->m2m.m2m_dev);
		goto err_dev_alloc;
	}

	ret = video_register_device(vfd, VFL_TYPE_PABLO,
				EXYNOS_VIDEONODE_CAMERAPP(CAMERAPP_VIDEONODE_GDC));
	if (ret) {
		dev_err(gdc->dev, "failed to register video device\n");
		goto err_m2m_dev;
	}

	return 0;

err_m2m_dev:
	v4l2_m2m_release(gdc->m2m.m2m_dev);
err_dev_alloc:
	video_device_release(gdc->m2m.vfd);
err_v4l2_dev:
	v4l2_device_unregister(v4l2_dev);

	return ret;
}
#ifdef CONFIG_EXYNOS_IOVMM
static int __attribute__((unused)) gdc_sysmmu_fault_handler(struct iommu_domain *domain,
	struct device *dev, unsigned long iova, int flags, void *token)
{
	struct gdc_dev *gdc = dev_get_drvdata(dev);
#else
static int gdc_sysmmu_fault_handler(struct iommu_fault *fault, void *data)
{
	struct gdc_dev *gdc = data;
	struct device *dev = gdc->dev;
	unsigned long iova = fault->event.addr;
#endif
	if (test_bit(DEV_RUN, &gdc->state)) {
		dev_info(dev, "System MMU fault called for IOVA %#lx\n", iova);
		camerapp_gdc_sfr_dump(gdc->regs_base);
	}

	return 0;
}

static int gdc_clk_get(struct gdc_dev *gdc)
{
	gdc->aclk = devm_clk_get(gdc->dev, "gate");
	if (IS_ERR(gdc->aclk)) {
		if (PTR_ERR(gdc->aclk) != -ENOENT) {
			dev_err(gdc->dev, "Failed to get 'gate' clock: %ld",
				PTR_ERR(gdc->aclk));
			return PTR_ERR(gdc->aclk);
		}
		dev_info(gdc->dev, "'gate' clock is not present\n");
	}

	gdc->pclk = devm_clk_get(gdc->dev, "gate2");
	if (IS_ERR(gdc->pclk)) {
		if (PTR_ERR(gdc->pclk) != -ENOENT) {
			dev_err(gdc->dev, "Failed to get 'gate2' clock: %ld",
				PTR_ERR(gdc->pclk));
			return PTR_ERR(gdc->pclk);
		}
		dev_info(gdc->dev, "'gate2' clock is not present\n");
	}

	gdc->clk_chld = devm_clk_get(gdc->dev, "mux_user");
	if (IS_ERR(gdc->clk_chld)) {
		if (PTR_ERR(gdc->clk_chld) != -ENOENT) {
			dev_err(gdc->dev, "Failed to get 'mux_user' clock: %ld",
				PTR_ERR(gdc->clk_chld));
			return PTR_ERR(gdc->clk_chld);
		}
		dev_info(gdc->dev, "'mux_user' clock is not present\n");
	}

	if (!IS_ERR(gdc->clk_chld)) {
		gdc->clk_parn = devm_clk_get(gdc->dev, "mux_src");
		if (IS_ERR(gdc->clk_parn)) {
			dev_err(gdc->dev, "Failed to get 'mux_src' clock: %ld",
				PTR_ERR(gdc->clk_parn));
			return PTR_ERR(gdc->clk_parn);
		}
	} else {
		gdc->clk_parn = ERR_PTR(-ENOENT);
	}

	return 0;
}

static void gdc_clk_put(struct gdc_dev *gdc)
{
	if (!IS_ERR(gdc->clk_parn))
		devm_clk_put(gdc->dev, gdc->clk_parn);

	if (!IS_ERR(gdc->clk_chld))
		devm_clk_put(gdc->dev, gdc->clk_chld);

	if (!IS_ERR(gdc->pclk))
		devm_clk_put(gdc->dev, gdc->pclk);

	if (!IS_ERR(gdc->aclk))
		devm_clk_put(gdc->dev, gdc->aclk);
}

#ifdef CONFIG_PM_SLEEP
static int gdc_suspend(struct device *dev)
{
	struct gdc_dev *gdc = dev_get_drvdata(dev);
	int ret;

	set_bit(DEV_SUSPEND, &gdc->state);

	ret = wait_event_timeout(gdc->wait,
			!test_bit(DEV_RUN, &gdc->state), GDC_TIMEOUT * 50); /*  2sec */
	if (ret == 0)
		dev_err(gdc->dev, "wait timeout\n");

	return 0;
}

static int gdc_resume(struct device *dev)
{
	struct gdc_dev *gdc = dev_get_drvdata(dev);

	clear_bit(DEV_SUSPEND, &gdc->state);

	return 0;
}
#endif

#ifdef CONFIG_PM
static int gdc_runtime_resume(struct device *dev)
{
	struct gdc_dev *gdc = dev_get_drvdata(dev);

	if (!IS_ERR(gdc->clk_chld) && !IS_ERR(gdc->clk_parn)) {
		int ret = clk_set_parent(gdc->clk_chld, gdc->clk_parn);
		if (ret) {
			dev_err(gdc->dev, "%s: Failed to setup MUX: %d\n",
				__func__, ret);
			return ret;
		}
	}

	if (gdc->qosreq_intcam_level > 0)
		gdc_pm_qos_update_request(&gdc->qosreq_intcam, gdc->qosreq_intcam_level);

	return 0;
}

static int gdc_runtime_suspend(struct device *dev)
{
	struct gdc_dev *gdc = dev_get_drvdata(dev);
	if (gdc->qosreq_intcam_level > 0)
		gdc_pm_qos_update_request(&gdc->qosreq_intcam, 0);
	return 0;
}
#endif

static const struct dev_pm_ops gdc_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(gdc_suspend, gdc_resume)
	SET_RUNTIME_PM_OPS(NULL, gdc_runtime_resume, gdc_runtime_suspend)
};

static int gdc_probe(struct platform_device *pdev)
{
	struct gdc_dev *gdc;
	struct resource *rsc;
	struct device_node *np;
	struct device_node *votf_np = NULL;
	int ret = 0;

	gdc = devm_kzalloc(&pdev->dev, sizeof(struct gdc_dev), GFP_KERNEL);
	if (!gdc) {
		dev_err(&pdev->dev, "no memory for GDC device\n");
		return -ENOMEM;
	}

	gdc->dev = &pdev->dev;
	np = gdc->dev->of_node;

	gdc_device = &pdev->dev;
	dev_set_drvdata(gdc_device, gdc);

	spin_lock_init(&gdc->ctxlist_lock);
	INIT_LIST_HEAD(&gdc->context_list);
	spin_lock_init(&gdc->slock);
	mutex_init(&gdc->lock);
	init_waitqueue_head(&gdc->wait);

	rsc = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gdc->regs_base = devm_ioremap(&pdev->dev, rsc->start, resource_size(rsc));
	if (IS_ERR(gdc->regs_base))
		return PTR_ERR(gdc->regs_base);

	rsc = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!rsc) {
		dev_err(&pdev->dev, "failed to get IRQ resource\n");
		return -ENOENT;
	}

	ret = devm_request_irq(&pdev->dev, rsc->start, gdc_irq_handler, 0,
			"GDC0", gdc);
	if (ret) {
		dev_err(&pdev->dev, "failed to install irq\n");
		return ret;
	}

	rsc = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (!rsc)
		dev_info(&pdev->dev, "there's no IRQ1 resource\n");
	else {
		ret = devm_request_irq(&pdev->dev, rsc->start,
				gdc_irq_handler, 0,
				"GDC1", gdc);
		if (ret) {
			dev_err(&pdev->dev, "failed to install irq\n");
			return ret;
		}
	}

	votf_np = of_get_child_by_name(np, "votf_axi");
	if (votf_np) {
		ret = of_property_read_u32_array(votf_np, "votf_src_dest", gdc->votf_src_dest, 2);
		if (ret) {
			gdc->has_votf_mfc = 0;
			dev_err(&pdev->dev, "failed to get votf src dest ID (ret:%d)\n", ret);
			return ret;
		}
		gdc->has_votf_mfc = 1;
		gdc->votf_base = ioremap(gdc->votf_src_dest[GDC_SRC_ADDR], 0x10000);
		gdc->mfc_votf_base = ioremap(gdc->votf_src_dest[GDC_DST_ADDR], 0x10000);
		gdc->domain = iommu_get_domain_for_dev(gdc->dev);
		iommu_map(gdc->domain, gdc->votf_src_dest[GDC_DST_ADDR],
			gdc->votf_src_dest[GDC_DST_ADDR], 0x10000, 0);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
		iommu_dma_reserve_iova(gdc->dev,
			gdc->votf_src_dest[GDC_DST_ADDR], 0x10000);
#endif
	} else {
		gdc->votf_src_dest[GDC_SRC_ADDR] = 0;
		gdc->votf_src_dest[GDC_DST_ADDR] = 0;
		gdc->has_votf_mfc = 0;
	}

	atomic_set(&gdc->wdt.cnt, 0);
	timer_setup(&gdc->wdt.timer, gdc_watchdog, 0);

	ret = gdc_clk_get(gdc);
	if (ret)
		return ret;

	if (pdev->dev.of_node)
		gdc->dev_id = of_alias_get_id(pdev->dev.of_node, "camerapp-gdc");
	else
		gdc->dev_id = pdev->id;

	platform_set_drvdata(pdev, gdc);

	pm_runtime_enable(&pdev->dev);

	ret = gdc_register_m2m_device(gdc, gdc->dev_id);
	if (ret) {
		dev_err(&pdev->dev, "failed to register m2m device\n");
		goto err_wq;
	}

#if IS_ENABLED(CONFIG_EXYNOS_PM_QOS)
	if (!of_property_read_u32(pdev->dev.of_node, "camerapp_gdc,intcam_qos_minlock",
				(u32 *)&gdc->qosreq_intcam_level)) {
		if (gdc->qosreq_intcam_level > 0) {
			gdc_pm_qos_add_request(&gdc->qosreq_intcam,
						PM_QOS_INTCAM_THROUGHPUT, 0);
			dev_info(&pdev->dev, "INTCAM Min.Lock Freq. = %u\n",
						gdc->qosreq_intcam_level);
		}
	}
#endif
/*
	if (of_property_read_u32(pdev->dev.of_node, "camerapp_gdc,cfw",
				(u32 *)&gdc->cfw))
		gdc->cfw = 0;

	if (of_find_property(pdev->dev.of_node, "prefetch-buffer,disable",
				NULL))
		gdc->pb_disable = true;
*/
#ifdef CONFIG_EXYNOS_IOVMM
	ret = iovmm_activate(gdc->dev);
	if (ret) {
		dev_err(&pdev->dev, "failed to attach iommu\n");
		goto err_iommu;
	}
#endif
	ret = pm_runtime_get_sync(&pdev->dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "%s: failed to local power on (err %d)\n",
			__func__, ret);
		goto err_ver_rpm_get;
	}

	if (!IS_ERR(gdc->pclk)) {
		ret = clk_prepare_enable(gdc->pclk);
		if (ret) {
			dev_err(&pdev->dev,
				"%s: failed to enable PCLK (err %d)\n",
				__func__, ret);
			goto err_ver_pclk_get;
		}
	}

	if (!IS_ERR(gdc->aclk)) {
		ret = clk_prepare_enable(gdc->aclk);
		if (ret) {
			dev_err(&pdev->dev,
				"%s: failed to enable ACLK (err %d)\n",
				__func__, ret);
			goto err_ver_aclk_get;
		}
	}

	gdc->version = camerapp_hw_gdc_get_ver(gdc->regs_base);
	gdc->variant = camerapp_hw_gdc_get_size_constraints(gdc->regs_base);
	gdc->stalled = 0;

	if (!IS_ERR(gdc->aclk))
		clk_disable_unprepare(gdc->aclk);
	if (!IS_ERR(gdc->pclk))
		clk_disable_unprepare(gdc->pclk);
	pm_runtime_put(&pdev->dev);
#ifdef CONFIG_EXYNOS_IOVMM
	iovmm_set_fault_handler(&pdev->dev, gdc_sysmmu_fault_handler, gdc);
#else
	iommu_register_device_fault_handler(&pdev->dev, gdc_sysmmu_fault_handler, gdc);
#endif
	dev_info(&pdev->dev,
		"Driver probed successfully(version: %08x)\n",
		gdc->version);

	return 0;

err_ver_aclk_get:
	if (!IS_ERR(gdc->pclk))
		clk_disable_unprepare(gdc->pclk);
err_ver_pclk_get:
	pm_runtime_put(&pdev->dev);
err_ver_rpm_get:
#ifdef CONFIG_EXYNOS_IOVMM
	iovmm_deactivate(gdc->dev);
err_iommu:
#endif
	if (gdc->qosreq_intcam_level > 0)
		gdc_pm_qos_remove_request(&gdc->qosreq_intcam);
	gdc_unregister_m2m_device(gdc);
err_wq:
	return ret;
}

static int gdc_remove(struct platform_device *pdev)
{
	struct gdc_dev *gdc = platform_get_drvdata(pdev);
#ifdef CONFIG_EXYNOS_IOVMM
	iovmm_deactivate(gdc->dev);
#else
	iommu_unregister_device_fault_handler(&pdev->dev);
#endif
	gdc_clk_put(gdc);

	if (timer_pending(&gdc->wdt.timer))
		del_timer(&gdc->wdt.timer);

	if (gdc->qosreq_intcam_level > 0)
		gdc_pm_qos_remove_request(&gdc->qosreq_intcam);

	if (gdc->has_votf_mfc) {
		iounmap(gdc->votf_base);
		iounmap(gdc->mfc_votf_base);
		iommu_unmap(gdc->domain, gdc->votf_src_dest[GDC_DST_ADDR], 0x10000);
	}

	return 0;
}

static void gdc_shutdown(struct platform_device *pdev)
{
	struct gdc_dev *gdc = platform_get_drvdata(pdev);

	set_bit(DEV_SUSPEND, &gdc->state);

	wait_event(gdc->wait,
			!test_bit(DEV_RUN, &gdc->state));

#ifdef CONFIG_EXYNOS_IOVMM
	iovmm_deactivate(gdc->dev);
#endif
}
static const struct of_device_id exynos_gdc_match[] = {
	{
		.compatible = "samsung,exynos-is-gdc",
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_gdc_match);

static struct platform_driver gdc_driver = {
	.probe		= gdc_probe,
	.remove		= gdc_remove,
	.shutdown	= gdc_shutdown,
	.driver = {
		.name	= GDC_MODULE_NAME,
		.owner	= THIS_MODULE,
		.pm	= &gdc_pm_ops,
		.of_match_table = of_match_ptr(exynos_gdc_match),
	}
};
builtin_platform_driver(gdc_driver);

MODULE_AUTHOR("SamsungLSI Camera");
MODULE_DESCRIPTION("EXYNOS CameraPP GDC driver");
MODULE_LICENSE("GPL");
