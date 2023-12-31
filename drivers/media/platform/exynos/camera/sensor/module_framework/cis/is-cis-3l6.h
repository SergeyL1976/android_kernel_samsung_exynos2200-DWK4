/*
 * Samsung Exynos5 SoC series Sensor driver
 *
 *
 * Copyright (c) 2021 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IS_CIS_3L6_H
#define IS_CIS_3L6_H

#include "is-cis.h"

#define EXT_CLK_Khz (19200)

#define SENSOR_3L6_MAX_WIDTH		(4000)
#define SENSOR_3L6_MAX_HEIGHT		(3000)

/* TODO: Check below values are valid */
#define SENSOR_3L6_FINE_INTEGRATION_TIME_MIN                0x1C5
#define SENSOR_3L6_FINE_INTEGRATION_TIME_MAX                0x1C5
#define SENSOR_3L6_COARSE_INTEGRATION_TIME_MIN              0x2
#define SENSOR_3L6_COARSE_INTEGRATION_TIME_MAX_MARGIN       0x4

/****
 **  Register Address
 **  : address name format: SENSOR_3L6_XX...XX_ADDR
 ****/
#define SENSOR_3L6_MODEL_ID_ADDR		(0x0000)
#define SENSOR_3L6_REVISION_NUMBER_ADDR		(0x0002)
#define SENSOR_3L6_FRAME_COUNT_ADDR		(0x0005)
#define SENSOR_3L6_MODE_SELECT_ADDR		(0x0100)
#define SENSOR_3L6_GROUP_PARAM_HOLD_ADDR	(0x0104)
#define SENSOR_3L6_COARSE_INTEGRATION_TIME_ADDR	(0x0202)
#define SENSOR_3L6_ANALOG_GAIN_ADDR		(0x0204)
#define SENSOR_3L6_DIGITAL_GAIN_ADDR		(0x020E)
#define SENSOR_3L6_FRAME_LENGTH_LINE_ADDR	(0x0340)
#define SENSOR_3L6_X_ADDR_START_ADDR		(0x0344)
#define SENSOR_3L6_Y_ADDR_START_ADDR		(0x0346)
#define SENSOR_3L6_X_ADDR_END_ADDR		(0x0348)
#define SENSOR_3L6_Y_ADDR_END_ADDR		(0x034A)
#define SENSOR_3L6_X_OUTPUT_SIZE_ADDR		(0x034C)
#define SENSOR_3L6_Y_OUTPUT_SIZE_ADDR		(0x034E)
#define SENSOR_3L6_X_EVEN_INC_ADDR		(0x0380)
#define SENSOR_3L6_X_ODD_INC_ADDR		(0x0382)
#define SENSOR_3L6_Y_EVEN_INC_ADDR		(0x0384)
#define SENSOR_3L6_Y_ODD_INC_ADDR		(0x0386)
#define SENSOR_3L6_SCALING_MODE_ADDR		(0x0400)
#define SENSOR_3L6_DOWN_SCALE_M_ADDR		(0x0404)
#define SENSOR_3L6_BINNING_MODE_ADDR		(0x0900)
#define SENSOR_3L6_BINNING_TYPE_ADDR		(0x0901)
#define SENSOR_3L6_PLL_POWER_CONTROL_ADDR	(0x3C1E)
#define SENSOR_3L6_PAGE_SELECT_ADDR		(0x6028)

#endif
