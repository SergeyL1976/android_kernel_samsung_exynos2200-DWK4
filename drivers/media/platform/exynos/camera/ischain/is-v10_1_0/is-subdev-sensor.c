// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Exynos SoC series Pablo driver
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "is-device-ischain.h"
#include "is-device-sensor.h"
#include "is-subdev-ctrl.h"
#include "is-config.h"
#include "is-param.h"
#include "is-video.h"
#include "is-type.h"

static int is_sensor_subdev_cfg(struct is_subdev *leader,
	void *device_data,
	struct is_frame *frame,
	struct is_crop *incrop,
	struct is_crop *otcrop,
	IS_DECLARE_PMAP(pmap))
{
	return 0;
}

static int is_sensor_subdev_tag(struct is_subdev *subdev,
	void *device_data,
	struct is_frame *frame,
	struct camera2_node *node)
{
	int ret = 0;
	struct is_device_sensor *device;

	device = (struct is_device_sensor *)device_data;
	if (!frame) {
		merr("[SEN] process is empty", device);
		goto p_err;
	}

p_err:
	return ret;
}


const struct is_subdev_ops is_subdev_sensor_ops = {
	.bypass	= NULL,
	.cfg	= is_sensor_subdev_cfg,
	.tag	= is_sensor_subdev_tag,
};

#if IS_ENABLED(CONFIG_PABLO_KUNIT_TEST)
const struct is_subdev_ops *pablo_get_is_subdev_sensor_ops(void)
{
	return &is_subdev_sensor_ops;
}
KUNIT_EXPORT_SYMBOL(pablo_get_is_subdev_sensor_ops);
#endif
