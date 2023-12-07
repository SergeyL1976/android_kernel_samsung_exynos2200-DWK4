/*
 * s2mps23-private.h - Voltage regulator driver for the s2mps23
 *
 *  Copyright (C) 2020 Samsung Electrnoics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __LINUX_MFD_S2MPS23_REGULATOR_H
#define __LINUX_MFD_S2MPS23_REGULATOR_H
#include <linux/i2c.h>

#if IS_ENABLED(CONFIG_S2MPS23_ADC)
#include <linux/iio/iio.h>
#include <linux/iio/machine.h>
#include <linux/iio/driver.h>
#include <linux/iio/consumer.h>
#endif

/* MIF_OUT registers */
#define MIF_OUT 	0x15863A20

/* WRSTBI registers */
#define PMUREG_SYSTEM_OUT		0x3C20		//AP PMU_ALIVE_BASE + 0x3A20
#define WRSTBI_MASK			(1 << 8)	//PAD__WRSTBI

#define S2MPS23_REG_INVALID             (0xFF)

/* PMIC Top-Level Registers addr*/
#define S2MPS23_PMIC_REG_VGPIO0		0x00
#define S2MPS23_PMIC_REG_VGPIO1		0x01
#define S2MPS23_PMIC_REG_VGPIO2		0x02
#define S2MPS23_PMIC_REG_VGPIO3		0x03
#define	S2MPS23_PMIC_REG_PMICID		0x0D
#define S2MPS23_PMIC_REG_I3C_CONFIG	0x0E
#define S2MPS23_PMIC_REG_I3C_STSA	0x0F
#define	S2MPS23_PMIC_REG_INTSRC		0x01
#define	S2MPS23_PMIC_REG_IRQM		0x10	/* IRQM reg */
#define S2MPS23_PMIC_PM_IRQM		(1 << 0)

/* PMIC CHIP ID H/W */
#define CHIP_ID_HW_MASK			(0x0F)

/* IBI table check*/
#define S2MPS23_IBI0_DATA_PARITY_ERR		(1 << 7)
#define S2MPS23_IBI0_ADDR_PARITY_ERR		(1 << 6)
#define S2MPS23_IBI0_CCC_PARIRY_ERR		(1 << 5)
#define S2MPS23_IBI0_PMIC_S			(1 << 0)
#define S2MPS23_IBI0_PMIC_M			(1 << 4)
#define S2MPS23_IBI1_ADC			(1 << 0)
#define S2MPS23_IBI1_DCXO			(1 << 1)
#define S2MPS23_IBI1_ONOB			(1 << 7)

/* PMIC Registers */
#define S2MPS23_PMIC_REG_INT1		0x00
#define S2MPS23_PMIC_REG_INT2		0x01
#define S2MPS23_PMIC_REG_INT3		0x02
#define S2MPS23_PMIC_REG_INT4		0x03
#define S2MPS23_PMIC_REG_INT5		0x04
#define S2MPS23_PMIC_REG_INT6		0x05
#define S2MPS23_PMIC_REG_INT7		0x06

#define S2MPS23_PMIC_REG_INT1M		0x07
#define S2MPS23_PMIC_REG_INT2M		0x08
#define S2MPS23_PMIC_REG_INT3M		0x09
#define S2MPS23_PMIC_REG_INT4M		0x0A
#define S2MPS23_PMIC_REG_INT5M		0x0B
#define S2MPS23_PMIC_REG_INT6M		0x0C
#define S2MPS23_PMIC_REG_INT7M		0x0D
#define S2MPS23_PMIC_REG_STATUS1	0x0E
#define S2MPS23_PMIC_REG_STATUS2	0x0F
#define S2MPS23_PMIC_REG_PWRONSRC	0x10
#define S2MPS23_PMIC_REG_OFFSRC1	0x11
#define S2MPS23_PMIC_REG_OFFSRC2	0x12

#define S2MPS23_PMIC_REG_BUCHG		0x13
#define S2MPS23_PMIC_REG_RTCBUF		0x14
#define S2MPS23_PMIC_REG_CTRL1		0x15
#define S2MPS23_PMIC_REG_CTRL2		0x16
#define S2MPS23_PMIC_REG_CTRL3		0x17

#define S2MPS23_PMIC_REG_ETC_OTP1	0x18
#define S2MPS23_PMIC_REG_ETC_OTP2	0x19
#define S2MPS23_PMIC_REG_UVLO_OTP	0x1A
#define S2MPS23_PMIC_REG_CFG1		0x1B
#define S2MPS23_PMIC_REG_CFG2		0x1C

#define S2MPS23_PMIC_REG_B1M_CTRL	0x1D
#define S2MPS23_PMIC_REG_B1M_OUT1	0x1E
#define S2MPS23_PMIC_REG_B2M_CTRL	0x1F
#define S2MPS23_PMIC_REG_B2M_OUT1	0x20
#define S2MPS23_PMIC_REG_B3M_CTRL	0x21
#define S2MPS23_PMIC_REG_B3M_OUT1	0x22
#define S2MPS23_PMIC_REG_B4M_CTRL	0x23
#define S2MPS23_PMIC_REG_B4M_OUT1	0x24
#define S2MPS23_PMIC_REG_B4M_OUT2	0x25
#define S2MPS23_PMIC_REG_B5M_CTRL	0x26
#define S2MPS23_PMIC_REG_B5M_OUT1	0x27
#define S2MPS23_PMIC_REG_B6M_CTRL	0x28
#define S2MPS23_PMIC_REG_B6M_OUT1	0x29
#define S2MPS23_PMIC_REG_B7M_CTRL	0x2A
#define S2MPS23_PMIC_REG_B7M_OUT1	0x2B
#define S2MPS23_PMIC_REG_B7M_OUT2	0x2C
#define S2MPS23_PMIC_REG_B8M_CTRL	0x2D
#define S2MPS23_PMIC_REG_B8M_OUT1	0x2E
#define S2MPS23_PMIC_REG_B9M_CTRL	0x2F
#define S2MPS23_PMIC_REG_B9M_OUT1	0x30

#define S2MPS23_PMIC_REG_BUCK_DVS1	0x31
#define S2MPS23_PMIC_REG_BUCK_DVS2	0x32
#define S2MPS23_PMIC_REG_BUCK_DVS3	0x33
#define S2MPS23_PMIC_REG_BUCK_DVS4	0x34
#define S2MPS23_PMIC_REG_BUCK_DVS5	0x35
#define S2MPS23_PMIC_REG_BUCK_DVS6	0x36
#define S2MPS23_PMIC_REG_BUCK_DVS7	0x37
#define S2MPS23_PMIC_REG_BUCK_DVS8	0x38
#define S2MPS23_PMIC_REG_BUCK_DVS9	0x39

#define S2MPS23_PMIC_REG_AVP_CTRL	0x3A

#define S2MPS23_PMIC_REG_DVS_LDO4_CTRL	0x3B
#define S2MPS23_PMIC_REG_DVS_LDO5_CTRL	0x3C
#define S2MPS23_PMIC_REG_DVS_LDO6_CTRL	0x3D
#define S2MPS23_PMIC_REG_DVS_LDO17_CTRL	0x3E
#define S2MPS23_PMIC_REG_DVS_LDO18_CTRL	0x3F
#define S2MPS23_PMIC_REG_DVS_LDO33_CTRL	0x40
#define S2MPS23_PMIC_REG_DVS_LDO_RAMP1	0x41
#define S2MPS23_PMIC_REG_DVS_LDO_RAMP2	0x42

#define S2MPS23_PMIC_REG_L1CTRL		0x43
#define S2MPS23_PMIC_REG_L2CTRL		0x44
#define S2MPS23_PMIC_REG_L3CTRL		0x45
#define S2MPS23_PMIC_REG_L4CTRL		0x46
#define S2MPS23_PMIC_REG_L5CTRL1	0x47
#define S2MPS23_PMIC_REG_L5CTRL2	0x48
#define S2MPS23_PMIC_REG_L6CTRL		0x49
#define S2MPS23_PMIC_REG_L7CTRL		0x4A
#define S2MPS23_PMIC_REG_L8CTRL		0x4B
#define S2MPS23_PMIC_REG_L9CTRL		0x4C
#define S2MPS23_PMIC_REG_L10CTRL	0x4D
#define S2MPS23_PMIC_REG_L11CTRL	0x4E
#define S2MPS23_PMIC_REG_L12CTRL	0x4F
#define S2MPS23_PMIC_REG_L13CTRL	0x50
#define S2MPS23_PMIC_REG_L14CTRL	0x51
#define S2MPS23_PMIC_REG_L15CTRL	0x52
#define S2MPS23_PMIC_REG_L16CTRL	0x53
#define S2MPS23_PMIC_REG_L17CTRL	0x54
#define S2MPS23_PMIC_REG_L18CTRL1	0x55
#define S2MPS23_PMIC_REG_L18CTRL2	0x56
#define S2MPS23_PMIC_REG_L19CTRL	0x57
#define S2MPS23_PMIC_REG_L20CTRL	0x58
#define S2MPS23_PMIC_REG_L21CTRL	0x59
#define S2MPS23_PMIC_REG_L22CTRL	0x5A
#define S2MPS23_PMIC_REG_L23CTRL	0x5B
#define S2MPS23_PMIC_REG_L24CTRL	0x5C
#define S2MPS23_PMIC_REG_L25CTRL	0x5D
#define S2MPS23_PMIC_REG_L26CTRL	0x5E
#define S2MPS23_PMIC_REG_L27CTRL	0x5F
#define S2MPS23_PMIC_REG_L28CTRL	0x60
#define S2MPS23_PMIC_REG_L29CTRL	0x61
#define S2MPS23_PMIC_REG_L30CTRL	0x62
#define S2MPS23_PMIC_REG_L31CTRL	0x63
#define S2MPS23_PMIC_REG_L32CTRL	0x64
#define S2MPS23_PMIC_REG_L33CTRL	0x65

#define S2MPS23_PMIC_REG_LDO_DSCH1	0x66
#define S2MPS23_PMIC_REG_LDO_DSCH2	0x67
#define S2MPS23_PMIC_REG_LDO_DSCH3	0x68
#define S2MPS23_PMIC_REG_LDO_DSCH4	0x69
#define S2MPS23_PMIC_REG_LDO_DSCH5	0x6A

#define S2MPS23_PMIC_REG_IOCONF		0x6B
#define S2MPS23_PMIC_REG_RSVD		0x6C
#define S2MPS23_PMIC_REG_IPTAT		0x6D

#define S2MPS23_PMIC_REG_ONSEQ_CTRL1	0x6E
#define S2MPS23_PMIC_REG_ONSEQ_CTRL2	0x6F
#define S2MPS23_PMIC_REG_ONSEQ_CTRL3	0x70
#define S2MPS23_PMIC_REG_ONSEQ_CTRL4	0x71
#define S2MPS23_PMIC_REG_ONSEQ_CTRL5	0x72
#define S2MPS23_PMIC_REG_ONSEQ_CTRL6	0x73
#define S2MPS23_PMIC_REG_ONSEQ_CTRL7	0x74
#define S2MPS23_PMIC_REG_ONSEQ_CTRL8	0x75
#define S2MPS23_PMIC_REG_ONSEQ_CTRL9	0x76
#define S2MPS23_PMIC_REG_ONSEQ_CTRL10	0x77
#define S2MPS23_PMIC_REG_ONSEQ_CTRL11	0x78
#define S2MPS23_PMIC_REG_ONSEQ_CTRL12	0x79
#define S2MPS23_PMIC_REG_ONSEQ_CTRL13	0x7A
#define S2MPS23_PMIC_REG_ONSEQ_CTRL14	0x7B
#define S2MPS23_PMIC_REG_ONSEQ_CTRL15	0x7C
#define S2MPS23_PMIC_REG_ONSEQ_CTRL16	0x7D
#define S2MPS23_PMIC_REG_ONSEQ_CTRL17	0x7E
#define S2MPS23_PMIC_REG_ONSEQ_CTRL18	0x7F
#define S2MPS23_PMIC_REG_ONSEQ_CTRL19	0x80
#define S2MPS23_PMIC_REG_ONSEQ_CTRL20	0x81
#define S2MPS23_PMIC_REG_ONSEQ_CTRL21	0x82
#define S2MPS23_PMIC_REG_ONSEQ_CTRL22	0x83
#define S2MPS23_PMIC_REG_ONSEQ_CTRL23	0x84
#define S2MPS23_PMIC_REG_ONSEQ_CTRL24	0x85
#define S2MPS23_PMIC_REG_ONSEQ_CTRL25	0x86
#define S2MPS23_PMIC_REG_ONSEQ_CTRL26	0x87
#define S2MPS23_PMIC_REG_ONSEQ_CTRL27	0x88
#define S2MPS23_PMIC_REG_ONSEQ_CTRL28	0x89
#define S2MPS23_PMIC_REG_ONSEQ_CTRL29	0x8A
#define S2MPS23_PMIC_REG_ONSEQ_CTRL30	0x8B
#define S2MPS23_PMIC_REG_ONSEQ_CTRL31	0x8C
#define S2MPS23_PMIC_REG_ONSEQ_CTRL32	0x8D
#define S2MPS23_PMIC_REG_ONSEQ_CTRL33	0x8E
#define S2MPS23_PMIC_REG_ONSEQ_CTRL34	0x8F
#define S2MPS23_PMIC_REG_ONSEQ_CTRL35	0x90

#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL1	0x91
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL2	0x92
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL3	0x93
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL4	0x94
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL5	0x95
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL6	0x96
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL7	0x97
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL8	0x98
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL9	0x99
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL10	0x9A
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL11	0x9B
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL12	0x9C
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL13	0x9D
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL14	0x9E
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL15	0x9F
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL16	0xA0
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL17	0xA1
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL18	0xA2
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL19	0xA3
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL20	0xA4
#define S2MPS23_PMIC_REG_OFF_SEQ_CTRL21	0xA5

#define S2MPS23_PMIC_REG_CONTROL_SEL1	0xA6
#define S2MPS23_PMIC_REG_CONTROL_SEL2	0xA7
#define S2MPS23_PMIC_REG_CONTROL_SEL3	0xA8
#define S2MPS23_PMIC_REG_CONTROL_SEL4	0xA9
#define S2MPS23_PMIC_REG_CONTROL_SEL5	0xAA
#define S2MPS23_PMIC_REG_CONTROL_SEL6	0xAB
#define S2MPS23_PMIC_REG_CONTROL_SEL7	0xAC
#define S2MPS23_PMIC_REG_CONTROL_SEL8	0xAD
#define S2MPS23_PMIC_REG_CONTROL_SEL9	0xAE
#define S2MPS23_PMIC_REG_CONTROL_SEL10	0xAF
#define S2MPS23_PMIC_REG_CONTROL_SEL11	0xB0
#define S2MPS23_PMIC_REG_CONTROL_SEL12	0xB1
#define S2MPS23_PMIC_REG_CONTROL_SEL13	0xB2
#define S2MPS23_PMIC_REG_CONTROL_SEL14	0xB3
#define S2MPS23_PMIC_REG_CONTROL_SEL15	0xB4
#define S2MPS23_PMIC_REG_CONTROL_SEL16	0xB5
#define S2MPS23_PMIC_REG_CONTROL_SEL17	0xB6
#define S2MPS23_PMIC_REG_CONTROL_SEL18	0xB7
#define S2MPS23_PMIC_REG_CONTROL_SEL19	0xB8
#define S2MPS23_PMIC_REG_CONTROL_SEL20	0xB9
#define S2MPS23_PMIC_REG_CONTROL_SEL21	0xBA

#define S2MPS23_PMIC_REG_OFF_CTRL1	0xBB
#define S2MPS23_PMIC_REG_OFF_CTRL2	0xBC
#define S2MPS23_PMIC_REG_OFF_CTRL3	0xBD
#define S2MPS23_PMIC_REG_OFF_CTRL4	0xBE
#define S2MPS23_PMIC_REG_OFF_CTRL5	0xBF
#define S2MPS23_PMIC_REG_SUB_CTRL	0xC0

#define S2MPS23_PMIC_REG_AFM_WARN1	0xC1
#define S2MPS23_PMIC_REG_AFM_WARN1_X	0xC2
#define S2MPS23_PMIC_REG_AFM_WARN1_Y	0xC3
#define S2MPS23_PMIC_REG_AFM_WARN1_Z	0xC4

#define S2MPS23_PMIC_REG_BUCK_OI_EN1	0xC5
#define S2MPS23_PMIC_REG_BUCK_OI_EN2	0xC6
#define S2MPS23_PMIC_REG_BUCK_OI_PD_EN1	0xC7
#define S2MPS23_PMIC_REG_BUCK_OI_PD_EN2	0xC8
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL1	0xC9
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL2	0xCA
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL3	0xCB
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL4	0xCC
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL5	0xCD
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL6	0xCE
#define S2MPS23_PMIC_REG_BUCK_OI_CTRL7	0xCF

#define S2MPS23_PMIC_REG_BUCK_OVP_EN1	0xD0
#define S2MPS23_PMIC_REG_BUCK_OVP_EN2	0xD1

#define S2MPS23_PMIC_REG_SEQ_CTRL	0xD2
#define S2MPS23_PMIC_REG_CFG_PM2	0xD3
#define S2MPS23_PMIC_REG_PSI_CTRL1	0xD4
#define S2MPS23_PMIC_REG_PSI_CTRL2	0xD5
#define S2MPS23_PMIC_REG_OFF_SEQ_SKIP	0xD6
#define S2MPS23_PMIC_REG_SEL_HW_GPIO	0xD7
#define S2MPS23_PMIC_REG_CFG_PM3	0xD8
#define S2MPS23_PMIC_REG_BUCK_PH_MODE	0xD9
#define S2MPS23_PMIC_REG_PROT_CTRL	0xDA

#define S2MPS23_PMIC_REG_M_VGPIO0	0xF0
#define S2MPS23_PMIC_REG_M_VGPIO1	0xF1
#define S2MPS23_PMIC_REG_M_VGPIO2	0xF2
#define S2MPS23_PMIC_REG_M_VGPIO3	0xF3
#define S2MPS23_PMIC_REG_EXT_CTRL	0xFB

/* regulator mask */
#define BUCK_RAMP_MASK			(0x03)

/* CONTROL_SEL */
#define S2MPS23_CONTROL_SEL_NUM		21
#define S2MPS23_CONTROL_SEL_MAX_VAL	0xFF
#define S2MPS23_PWREN_MIF_MASK		0x00
#define S2MPS23_PWREN_CPUCL0_MASK	0x01
#define S2MPS23_PWREN_CPUCL1_MASK	0x02
#define S2MPS23_PWREN_CPUCL2_MASK	0x03
#define S2MPS23_PWREN_G3D_MASK		0x04
#define S2MPS23_PWREN_NPU_MASK		0x05
#define S2MPS23_PWREN_CP_MASK		0x06
#define S2MPS23_PWREN_CLK_MASK		0x07
#define S2MPS23_PWREN_RF0_MASK		0x08
#define S2MPS23_PWREN_RF1_MASK		0x09
#define S2MPS23_PWREN_NFC_MASK		0x0A
#define S2MPS23_PWREN_CPUCL0_CPUCL1_MASK	0x0C
#define S2MPS23_PWREN_CPUCL0_CPUCL2_MASK	0x0D
#define S2MPS23_PWREN_RF0_RF1_MASK		0x0E
#define S2MPS23_PWREN_CLK_RF0_RF1_NFC_MASK	0x0F

/* S2MPS23 regulator ids */
enum S2MPS23_regulators {
	S2MPS23_LDO1,
	S2MPS23_LDO2,
/*
	S2MPS23_LDO3,
	S2MPS23_LDO4,
	S2MPS23_LDO5,
*/
	S2MPS23_LDO6,
/*
	S2MPS23_LDO7,
*/
	S2MPS23_LDO8,
	S2MPS23_LDO9,
	S2MPS23_LDO10,
	S2MPS23_LDO11,
	S2MPS23_LDO12,
	S2MPS23_LDO13,
/*
	S2MPS23_LDO14,
	S2MPS23_LDO15,
	S2MPS23_LDO16,
	S2MPS23_LDO17,
	S2MPS23_LDO18,
*/
	S2MPS23_LDO19,
/*
	S2MPS23_LDO20,
	S2MPS23_LDO21,
*/
	S2MPS23_LDO22,
	S2MPS23_LDO23,
	S2MPS23_LDO24,
	S2MPS23_LDO25,
	S2MPS23_LDO26,
	S2MPS23_LDO27,
	S2MPS23_LDO28,
	S2MPS23_LDO29,
	S2MPS23_LDO30,
	S2MPS23_LDO31,
	S2MPS23_LDO32,
/*
	S2MPS23_LDO33,
*/
	S2MPS23_BUCK1,
/*
	S2MPS23_BUCK2,
	S2MPS23_BUCK3,
*/
	S2MPS23_BUCK4,
	S2MPS23_BUCK5,
	S2MPS23_BUCK6,
	S2MPS23_BUCK7,
	S2MPS23_BUCK8,
	S2MPS23_BUCK9,
	S2MPS23_REG_MAX,
};

/* BUCKs 1M~8M */
#define S2MPS23_BUCK_MIN1	300000
#define S2MPS23_BUCK_STEP1	6250
/* BUCK 9M  */
#define S2MPS23_BUCK_MIN2	600000
#define S2MPS23_BUCK_STEP2	12500
/* LDOs 1M/7M/8M/9M/10M/13M/14M */
#define S2MPS23_LDO_MIN1	725000
#define S2MPS23_LDO_STEP1	12500
/* LDOs 3M */
#define S2MPS23_LDO_MIN2	425000
#define S2MPS23_LDO_STEP2	12500
/* LDOs 4M/5M/6M/17M/18M/33M */
#define S2MPS23_LDO_MIN3	300000
#define S2MPS23_LDO_STEP3	25000
/* LDOs 2M/11M/12M/15M/16M/19M/20M/21M/22M/23M/24M/25M/
   	26M/27M/28M/29M/30M/31M/32M */
#define S2MPS23_LDO_MIN4	700000
#define S2MPS23_LDO_STEP4	25000
/* changed EVT1.1 LDO1M, LDO8M */
/* LDOs 1M/3M/8M */
#define S2MPS23_LDO_MIN2_EVT1_1		425000
#define S2MPS23_LDO_STEP2_EVT1_1	12500

/* LDO/BUCK output voltage control */
#define S2MPS23_LDO_VSEL_MASK	0x3F	/* LDO_CTRL */
#define S2MPS23_BUCK_VSEL_MASK	0xFF	/* BUCK_OUT */
#define S2MPS23_LDO_N_VOLTAGES	(S2MPS23_LDO_VSEL_MASK + 1)
#define S2MPS23_BUCK_N_VOLTAGES (S2MPS23_BUCK_VSEL_MASK + 1)

/* Buck/LDO Enable control [7:6] */
#define S2MPS23_ENABLE_SHIFT	0x06
#define S2MPS23_ENABLE_MASK	(0x03 << S2MPS23_ENABLE_SHIFT)
#define CONTROL_SEL_ON		(0x01 << S2MPS23_ENABLE_SHIFT)
#define CONTROL_SEL_NORMAL	(0x02 << S2MPS23_ENABLE_SHIFT)

/* soft start time */
#define S2MPS23_ENABLE_TIME_LDO		128
#define S2MPS23_ENABLE_TIME_BUCK	130

#define S2MPS23_REGULATOR_MAX (S2MPS23_REG_MAX)
#define SEC_PMIC_REV(iodev)    (iodev)->pmic_rev

#define S2MPS23_AFM_WARN_EN_SHIFT		7
#define S2MPS23_AFM_WARN_CNT_SHIFT		6
#define S2MPS23_AFM_WARN_DVS_MASK_SHIFT		5
#define S2MPS23_AFM_WARN_LV_SHIFT		0

/* VGPIO_RX_MONITOR ADDR. */
#define VGPIO_I3C_BASE			0x15960000
#define VGPIO_MONITOR_ADDR		0x1704

/* VGPIO_PENDING_CLEAR */
#define SYSREG_VGPIO2AP			0x15980000
#define INTC0_IPRIO_PEND		0x0290

/* POWER-KEY MASK */
#define S2MPS23_STATUS1_PWRON		(1 << 0)
#define S2MPS23_RISING_EDGE		(1 << 1)
#define S2MPS23_FALLING_EDGE		(1 << 0)
/*
 * sec_opmode_data - regulator operation mode data
 * @id: regulator id
 * @mode: regulator operation mode
 */

enum s2mps23_irq_source {
	S2MPS23_PMIC_INT1 = 0,
	S2MPS23_PMIC_INT2,
	S2MPS23_PMIC_INT3,
	S2MPS23_PMIC_INT4,
	S2MPS23_PMIC_INT5,
	S2MPS23_PMIC_INT6,
	S2MPS23_PMIC_INT7,

	S2MPS23_IRQ_GROUP_NR,
};

#define S2MPS23_NUM_IRQ_PMIC_REGS	7
#define S2MPS23_BUCK_MAX		9
#define S2MPS23_BUCK6M_OCP_IDX		5	// OCP array index
#define S2MPS23_TEMP_MAX		2

enum s2mps23_irq {
	/* PMIC */
	S2MPS23_PMIC_IRQ_PWRONF_INT1,
	S2MPS23_PMIC_IRQ_PWRONR_INT1,
	S2MPS23_PMIC_IRQ_JIGONBF_INT1,
	S2MPS23_PMIC_IRQ_JIGONBR_INT1,
	S2MPS23_PMIC_IRQ_ACOKBF_INT1,
	S2MPS23_PMIC_IRQ_ACOKBR_INT1,
	S2MPS23_PMIC_IRQ_PWRON1S_INT1,
	S2MPS23_PMIC_IRQ_MRB_INT1,

	S2MPS23_PMIC_IRQ_RTC60S_INT2,
	S2MPS23_PMIC_IRQ_RTCA1_INT2,
	S2MPS23_PMIC_IRQ_RTCA0_INT2,
	S2MPS23_PMIC_IRQ_SMPL_INT2,
	S2MPS23_PMIC_IRQ_RTC1S_INT2,
	S2MPS23_PMIC_IRQ_WTSR_INT2,
	S2MPS23_PMIC_IRQ_WRSTB_INT2,

	S2MPS23_PMIC_IRQ_120C_INT3,
	S2MPS23_PMIC_IRQ_140C_INT3,
	S2MPS23_PMIC_IRQ_TSD_INT3,
	S2MPS23_PMIC_IRQ_OVP_INT3,
	S2MPS23_PMIC_IRQ_TIMEOUT2_INT3,
	S2MPS23_PMIC_IRQ_TIMEOUT3_INT3,
	S2MPS23_PMIC_IRQ_SUB_SMPL_INT3,

	S2MPS23_PMIC_IRQ_OCP_B1M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B2M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B3M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B4M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B5M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B6M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B7M_INT4,
	S2MPS23_PMIC_IRQ_OCP_B8M_INT4,

	S2MPS23_PMIC_IRQ_OCP_B9M_INT5,
	S2MPS23_PMIC_IRQ_PARITY_ERR0_INT5,
	S2MPS23_PMIC_IRQ_PARITY_ERR1_INT5,
	S2MPS23_PMIC_IRQ_PARITY_ERR2_INT5,
	S2MPS23_PMIC_IRQ_LDO28_SCP_INT5,

	S2MPS23_PMIC_IRQ_OI_B1M_INT6,
	S2MPS23_PMIC_IRQ_OI_B2M_INT6,
	S2MPS23_PMIC_IRQ_OI_B3M_INT6,
	S2MPS23_PMIC_IRQ_OI_B4M_INT6,
	S2MPS23_PMIC_IRQ_OI_B5M_INT6,
	S2MPS23_PMIC_IRQ_OI_B6M_INT6,
	S2MPS23_PMIC_IRQ_OI_B7M_INT6,
	S2MPS23_PMIC_IRQ_OI_B8M_INT6,

	S2MPS23_PMIC_IRQ_OI_B9M_INT7,

	S2MPS23_IRQ_NR,
};

enum sec_device_type {
	S2MPS23X,
};

struct s2mps23_dev {
	struct device *dev;
	struct i2c_client *i2c;
	struct i2c_client *pmic;
	struct i2c_client *rtc;
	struct i2c_client *debug_i2c;
	struct mutex i2c_lock;
	struct apm_ops *ops;

	int type;
	int device_type;
	int irq;
	int irq_base;
	bool wakeup;
	struct mutex irqlock;
	int irq_masks_cur[S2MPS23_IRQ_GROUP_NR];
	int irq_masks_cache[S2MPS23_IRQ_GROUP_NR];

	/* pmic VER/REV register */
	u8 pmic_rev;	/* pmic Rev */

	struct s2mps23_platform_data *pdata;
#if IS_ENABLED(CONFIG_DRV_SAMSUNG_PMIC)
	struct device *powermeter_dev;
#endif
	/* VGPIO_RX_MONITOR */
	void __iomem *mem_base;

	/* Work queue */
	struct workqueue_struct *irq_wqueue;
	struct delayed_work irq_work;

#if IS_ENABLED(CONFIG_S2MPS23_ADC)
	struct i2c_client *adc_i2c;
#endif
	/* power meter */
	struct adc_info *adc_meter;
	int adc_mode;
};

enum s2mps23_types {
	TYPE_S2MPS23,
};

extern int s2mps23_adc_set_enable(struct adc_info *adc_meter, int en);

extern int s2mps23_irq_init(struct s2mps23_dev *s2mps23);
extern void s2mps23_irq_exit(struct s2mps23_dev *s2mps23);

extern void s2mps23_powermeter_init(struct s2mps23_dev *s2mps23);
extern void s2mps23_powermeter_deinit(struct s2mps23_dev *s2mps23);

/* S2MPS23 shared i2c API function */
extern int s2mps23_read_reg(struct i2c_client *i2c, u8 reg, u8 *dest);
extern int s2mps23_bulk_read(struct i2c_client *i2c, u8 reg, int count,
				u8 *buf);
extern int s2mps23_write_reg(struct i2c_client *i2c, u8 reg, u8 value);
extern int s2mps23_bulk_write(struct i2c_client *i2c, u8 reg, int count,
				u8 *buf);
extern int s2mps23_write_word(struct i2c_client *i2c, u8 reg, u16 value);
extern int s2mps23_read_word(struct i2c_client *i2c, u8 reg);

extern int s2mps23_update_reg(struct i2c_client *i2c, u8 reg, u8 val, u8 mask);

extern int pmic_read_pwrkey_status(void);
#if IS_ENABLED(CONFIG_MFD_S2MPS24)
extern void s2mps24_call_notifier(void);
#endif
#endif /* __LINUX_MFD_S2MPS23_REGULATOR_H */
