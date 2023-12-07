/*
 * Copyright (c) 2018 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _RX_REO_QUEUE_H_
#define _RX_REO_QUEUE_H_
#if !defined(__ASSEMBLER__)
#endif

#include "uniform_descriptor_header.h"

// ################ START SUMMARY #################
//
//	Dword	Fields
//	0	struct uniform_descriptor_header descriptor_header;
//	1	receive_queue_number[15:0], reserved_1b[31:16]
//	2	vld[0], associated_link_descriptor_counter[2:1], disable_duplicate_detection[3], soft_reorder_enable[4], ac[6:5], bar[7], rty[8], chk_2k_mode[9], oor_mode[10], ba_window_size[18:11], pn_check_needed[19], pn_shall_be_even[20], pn_shall_be_uneven[21], pn_handling_enable[22], pn_size[24:23], ignore_ampdu_flag[25], reserved_2b[31:26]
//	3	svld[0], ssn[12:1], current_index[20:13], seq_2k_error_detected_flag[21], pn_error_detected_flag[22], reserved_3a[30:23], pn_valid[31]
//	4	pn_31_0[31:0]
//	5	pn_63_32[31:0]
//	6	pn_95_64[31:0]
//	7	pn_127_96[31:0]
//	8	last_rx_enqueue_timestamp[31:0]
//	9	last_rx_dequeue_timestamp[31:0]
//	10	ptr_to_next_aging_queue_31_0[31:0]
//	11	ptr_to_next_aging_queue_39_32[7:0], reserved_11a[31:8]
//	12	ptr_to_previous_aging_queue_31_0[31:0]
//	13	ptr_to_previous_aging_queue_39_32[7:0], reserved_13a[31:8]
//	14	rx_bitmap_31_0[31:0]
//	15	rx_bitmap_63_32[31:0]
//	16	rx_bitmap_95_64[31:0]
//	17	rx_bitmap_127_96[31:0]
//	18	rx_bitmap_159_128[31:0]
//	19	rx_bitmap_191_160[31:0]
//	20	rx_bitmap_223_192[31:0]
//	21	rx_bitmap_255_224[31:0]
//	22	current_mpdu_count[6:0], current_msdu_count[31:7]
//	23	reserved_23[3:0], timeout_count[9:4], forward_due_to_bar_count[15:10], duplicate_count[31:16]
//	24	frames_in_order_count[23:0], bar_received_count[31:24]
//	25	mpdu_frames_processed_count[31:0]
//	26	msdu_frames_processed_count[31:0]
//	27	total_processed_byte_count[31:0]
//	28	late_receive_mpdu_count[11:0], window_jump_2k[15:12], hole_count[31:16]
//	29	reserved_29[31:0]
//	30	reserved_30[31:0]
//	31	reserved_31[31:0]
//
// ################ END SUMMARY #################

#define NUM_OF_DWORDS_RX_REO_QUEUE 32

struct rx_reo_queue {
    struct            uniform_descriptor_header                       descriptor_header;
             uint32_t receive_queue_number            : 16, //[15:0]
                      reserved_1b                     : 16; //[31:16]
             uint32_t vld                             :  1, //[0]
                      associated_link_descriptor_counter:  2, //[2:1]
                      disable_duplicate_detection     :  1, //[3]
                      soft_reorder_enable             :  1, //[4]
                      ac                              :  2, //[6:5]
                      bar                             :  1, //[7]
                      rty                             :  1, //[8]
                      chk_2k_mode                     :  1, //[9]
                      oor_mode                        :  1, //[10]
                      ba_window_size                  :  8, //[18:11]
                      pn_check_needed                 :  1, //[19]
                      pn_shall_be_even                :  1, //[20]
                      pn_shall_be_uneven              :  1, //[21]
                      pn_handling_enable              :  1, //[22]
                      pn_size                         :  2, //[24:23]
                      ignore_ampdu_flag               :  1, //[25]
                      reserved_2b                     :  6; //[31:26]
             uint32_t svld                            :  1, //[0]
                      ssn                             : 12, //[12:1]
                      current_index                   :  8, //[20:13]
                      seq_2k_error_detected_flag      :  1, //[21]
                      pn_error_detected_flag          :  1, //[22]
                      reserved_3a                     :  8, //[30:23]
                      pn_valid                        :  1; //[31]
             uint32_t pn_31_0                         : 32; //[31:0]
             uint32_t pn_63_32                        : 32; //[31:0]
             uint32_t pn_95_64                        : 32; //[31:0]
             uint32_t pn_127_96                       : 32; //[31:0]
             uint32_t last_rx_enqueue_timestamp       : 32; //[31:0]
             uint32_t last_rx_dequeue_timestamp       : 32; //[31:0]
             uint32_t ptr_to_next_aging_queue_31_0    : 32; //[31:0]
             uint32_t ptr_to_next_aging_queue_39_32   :  8, //[7:0]
                      reserved_11a                    : 24; //[31:8]
             uint32_t ptr_to_previous_aging_queue_31_0: 32; //[31:0]
             uint32_t ptr_to_previous_aging_queue_39_32:  8, //[7:0]
                      reserved_13a                    : 24; //[31:8]
             uint32_t rx_bitmap_31_0                  : 32; //[31:0]
             uint32_t rx_bitmap_63_32                 : 32; //[31:0]
             uint32_t rx_bitmap_95_64                 : 32; //[31:0]
             uint32_t rx_bitmap_127_96                : 32; //[31:0]
             uint32_t rx_bitmap_159_128               : 32; //[31:0]
             uint32_t rx_bitmap_191_160               : 32; //[31:0]
             uint32_t rx_bitmap_223_192               : 32; //[31:0]
             uint32_t rx_bitmap_255_224               : 32; //[31:0]
             uint32_t current_mpdu_count              :  7, //[6:0]
                      current_msdu_count              : 25; //[31:7]
             uint32_t reserved_23                     :  4, //[3:0]
                      timeout_count                   :  6, //[9:4]
                      forward_due_to_bar_count        :  6, //[15:10]
                      duplicate_count                 : 16; //[31:16]
             uint32_t frames_in_order_count           : 24, //[23:0]
                      bar_received_count              :  8; //[31:24]
             uint32_t mpdu_frames_processed_count     : 32; //[31:0]
             uint32_t msdu_frames_processed_count     : 32; //[31:0]
             uint32_t total_processed_byte_count      : 32; //[31:0]
             uint32_t late_receive_mpdu_count         : 12, //[11:0]
                      window_jump_2k                  :  4, //[15:12]
                      hole_count                      : 16; //[31:16]
             uint32_t reserved_29                     : 32; //[31:0]
             uint32_t reserved_30                     : 32; //[31:0]
             uint32_t reserved_31                     : 32; //[31:0]
};

/*

struct uniform_descriptor_header descriptor_header
			
			Details about which module owns this struct.
			
			Note that sub field Buffer_type shall be set to
			Receive_REO_queue_descriptor

receive_queue_number
			
			Indicates the MPDU queue ID to which this MPDU link
			descriptor belongs
			
			Used for tracking and debugging
			
			<legal all>

reserved_1b
			
			<legal 0>

vld
			
			Valid bit indicating a session is established and the
			queue descriptor is valid(Filled by SW)
			
			<legal all>

associated_link_descriptor_counter
			
			Indicates which of the 3 link descriptor counters shall
			be incremented or decremented when link descriptors are
			added or removed from this flow queue.
			
			MSDU link descriptors related with MPDUs stored in the
			re-order buffer shall also be included in this count.
			
			
			
			<legal 0-2>

disable_duplicate_detection
			
			When set, do not perform any duplicate detection.
			
			
			
			<legal all>

soft_reorder_enable
			
			When set, REO has been instructed to not perform the
			actual re-ordering of frames for this queue, but just to
			insert the reorder opcodes.
			
			
			
			Note that this implies that REO is also not going to
			perform any MSDU level operations, and the entire MPDU (and
			thus pointer to the MSDU link descriptor) will be pushed to
			a destination ring that SW has programmed in a SW
			programmable configuration register in REO
			
			
			
			<legal all>

ac
			
			Indicates which access category the queue descriptor
			belongs to(filled by SW)
			
			<legal all>

bar
			
			Indicates if  BAR has been received (mostly used for
			debug purpose and this is filled by REO)
			
			<legal all>

rty
			
			Retry bit is checked if this bit is set.  
			
			<legal all>

chk_2k_mode
			
			Indicates what type of operation is expected from Reo
			when the received frame SN falls within the 2K window
			
			
			
			See REO MLD document for programming details.
			
			<legal all>

oor_mode
			
			Out of Order mode:
			
			Indicates what type of operation is expected when the
			received frame falls within the OOR window.
			
			
			
			See REO MLD document for programming details.
			
			<legal all>

ba_window_size
			
			Indicates the negotiated (window size + 1). 
			
			it can go up to Max of 256bits.
			
			
			
			A value 255 means 256 bitmap, 63 means 64 bitmap, 0
			(means non-BA session, with window size of 0). The 3 values
			here are the main values validated, but other values should
			work as well.
			
			
			
			A BA window size of 0 (=> one frame entry bitmat), means
			that there is NO RX_REO_QUEUE_EXT descriptor following this
			RX_REO_QUEUE STRUCT in memory
			
			
			
			A BA window size of 1 - 105, means that there is 1
			RX_REO_QUEUE_EXT descriptor directly following this
			RX_REO_QUEUE STRUCT in memory.
			
			
			
			A BA window size of 106 - 210, means that there are 2
			RX_REO_QUEUE_EXT descriptors directly following this
			RX_REO_QUEUE STRUCT in memory
			
			
			
			A BA window size of 211 - 256, means that there are 3
			RX_REO_QUEUE_EXT descriptors directly following this
			RX_REO_QUEUE STRUCT in memory
			
			
			
			<legal 0 - 255>

pn_check_needed
			
			When set, REO shall perform the PN increment check
			
			<legal all>

pn_shall_be_even
			
			Field only valid when 'pn_check_needed' is set.
			
			
			
			When set, REO shall confirm that the received PN number
			is not only incremented, but also always an even number
			
			<legal all>

pn_shall_be_uneven
			
			Field only valid when 'pn_check_needed' is set.
			
			
			
			When set, REO shall confirm that the received PN number
			is not only incremented, but also always an uneven number
			
			<legal all>

pn_handling_enable
			
			Field only valid when 'pn_check_needed' is set.
			
			
			
			When set, and REO detected a PN error, HW shall set the
			'pn_error_detected_flag'.
			
			<legal all>

pn_size
			
			Size of the PN field check.
			
			Needed for wrap around handling...
			
			
			
			<enum 0     pn_size_24>
			
			<enum 1     pn_size_48>
			
			<enum 2     pn_size_128>
			
			
			
			<legal 0-2>

ignore_ampdu_flag
			
			When set, REO shall ignore the ampdu_flag on the
			entrance descriptor for this queue.
			
			<legal all>

reserved_2b
			
			<legal 0>

svld
			
			Sequence number in next field is valid one. It can be
			filled by SW if the want to fill in the any negotiated SSN,
			otherwise REO will fill the sequence number of first
			received packet and set this bit to 1.
			
			<legal all>

ssn
			
			Starting Sequence number of the session, this changes
			whenever window moves. (can be filled by SW then maintained
			by REO)
			
			<legal all>

current_index
			
			Points to last forwarded packet
			
			<legal all>

seq_2k_error_detected_flag
			
			Set by REO, can only be cleared by SW
			
			
			
			When set, REO has detected a 2k error jump in the
			sequence number and from that moment forward, all new frames
			are forwarded directly to FW, without duplicate detect,
			reordering, etc.
			
			<legal all>

pn_error_detected_flag
			
			Set by REO, can only be cleared by SW
			
			
			
			When set, REO has detected a PN error and from that
			moment forward, all new frames are forwarded directly to FW,
			without duplicate detect, reordering, etc.
			
			<legal all>

reserved_3a
			
			<legal 0>

pn_valid
			
			PN number in next fields are valid. It can be filled by
			SW if it wants to fill in the any negotiated SSN, otherwise
			REO will fill the pn based on the first received packet and
			set this bit to 1.
			
			<legal all>

pn_31_0
			
			
			<legal all>

pn_63_32
			
			Bits [63:32] of the PN number.  
			
			<legal all> 

pn_95_64
			
			Bits [95:64] of the PN number.  
			
			<legal all>

pn_127_96
			
			Bits [127:96] of the PN number.  
			
			<legal all>

last_rx_enqueue_timestamp
			
			This timestamp is updated when an MPDU is received and
			accesses this Queue Descriptor. It does not include the
			access due to Command TLVs or Aging (which will be updated
			in Last_rx_dequeue_timestamp).
			
			<legal all>

last_rx_dequeue_timestamp
			
			This timestamp is used for Aging. When an MPDU or
			multiple MPDUs are forwarded, either due to window movement,
			bar, aging or command flush, this timestamp is updated. Also
			when the bitmap is all zero and the first time an MPDU is
			queued (opcode=QCUR), this timestamp is updated for aging.
			
			<legal all>

ptr_to_next_aging_queue_31_0
			
			Address  (address bits 31-0)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the last entry in the list.
			
			<legal all>

ptr_to_next_aging_queue_39_32
			
			Address  (address bits 39-32)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the last entry in the list.
			
			<legal all>

reserved_11a
			
			<legal 0>

ptr_to_previous_aging_queue_31_0
			
			Address  (address bits 31-0)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the first entry in the list.
			
			<legal all>

ptr_to_previous_aging_queue_39_32
			
			Address  (address bits 39-32)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the first entry in the list.
			
			<legal all>

reserved_13a
			
			<legal 0>

rx_bitmap_31_0
			
			When a bit is set, the corresponding frame is currently
			held in the re-order queue.
			
			The bitmap  is Fully managed by HW. 
			
			SW shall init this to 0, and then never ever change it
			
			<legal all>

rx_bitmap_63_32
			
			See Rx_bitmap_31_0 description
			
			<legal all>

rx_bitmap_95_64
			
			See Rx_bitmap_31_0 description
			
			<legal all>

rx_bitmap_127_96
			
			See Rx_bitmap_31_0 description
			
			<legal all>

rx_bitmap_159_128
			
			See Rx_bitmap_31_0 description
			
			<legal all>

rx_bitmap_191_160
			
			See Rx_bitmap_31_0 description
			
			<legal all>

rx_bitmap_223_192
			
			See Rx_bitmap_31_0 description
			
			<legal all>

rx_bitmap_255_224
			
			See Rx_bitmap_31_0 description
			
			<legal all>

current_mpdu_count
			
			The number of MPDUs in the queue.
			
			
			
			<legal all>

current_msdu_count
			
			The number of MSDUs in the queue.
			
			<legal all>

reserved_23
			
			<legal 0>

timeout_count
			
			The number of times that REO started forwarding frames
			even though there is a hole in the bitmap. Forwarding reason
			is Timeout
			
			
			
			The counter saturates and freezes at 0x3F
			
			
			
			<legal all>

forward_due_to_bar_count
			
			The number of times that REO started forwarding frames
			even though there is a hole in the bitmap. Forwarding reason
			is reception of BAR frame.
			
			
			
			The counter saturates and freezes at 0x3F
			
			
			
			<legal all>

duplicate_count
			
			The number of duplicate frames that have been detected
			
			<legal all>

frames_in_order_count
			
			The number of frames that have been received in order
			(without a hole that prevented them from being forwarded
			immediately)
			
			
			
			This corresponds to the Reorder opcodes:
			
			'FWDCUR' and 'FWD BUF'
			
			
			
			<legal all>

bar_received_count
			
			The number of times a BAR frame is received.
			
			
			
			This corresponds to the Reorder opcodes with 'DROP'
			
			
			
			The counter saturates and freezes at 0xFF
			
			<legal all>

mpdu_frames_processed_count
			
			The total number of MPDU frames that have been processed
			by REO. 'Processing' here means that REO has received them
			out of the entrance ring, and retrieved the corresponding
			RX_REO_QUEUE Descriptor. 
			
			
			
			Note that this count includes duplicates, frames that
			later had errors, etc.
			
			
			
			Note that field 'Duplicate_count' indicates how many of
			these MPDUs were duplicates.
			
			
			
			<legal all>

msdu_frames_processed_count
			
			The total number of MSDU frames that have been processed
			by REO. 'Processing' here means that REO has received them
			out of the entrance ring, and retrieved the corresponding
			RX_REO_QUEUE Descriptor. 
			
			
			
			Note that this count includes duplicates, frames that
			later had errors, etc.
			
			
			
			<legal all>

total_processed_byte_count
			
			An approximation of the number of bytes processed for
			this queue. 
			
			'Processing' here means that REO has received them out
			of the entrance ring, and retrieved the corresponding
			RX_REO_QUEUE Descriptor. 
			
			
			
			Note that this count includes duplicates, frames that
			later had errors, etc.
			
			
			
			In 64 byte units
			
			<legal all>

late_receive_mpdu_count
			
			The number of MPDUs received after the window had
			already moved on. The 'late' sequence window is defined as
			(Window SSN - 256) - (Window SSN - 1)
			
			
			
			This corresponds with Out of order detection in
			duplicate detect FSM
			
			
			
			The counter saturates and freezes at 0xFFF
			
			
			
			<legal all>

window_jump_2k
			
			The number of times the window moved more then 2K
			
			
			
			The counter saturates and freezes at 0xF
			
			
			
			(Note: field name can not start with number: previous
			2k_window_jump)
			
			
			
			<legal all>

hole_count
			
			The number of times a hole was created in the receive
			bitmap.
			
			
			
			This corresponds to the Reorder opcodes with 'QCUR'
			
			
			
			<legal all>

reserved_29
			
			<legal 0>

reserved_30
			
			<legal 0>

reserved_31
			
			<legal 0>
*/

#define RX_REO_QUEUE_0_UNIFORM_DESCRIPTOR_HEADER_DESCRIPTOR_HEADER_OFFSET 0x00000000
#define RX_REO_QUEUE_0_UNIFORM_DESCRIPTOR_HEADER_DESCRIPTOR_HEADER_LSB 0
#define RX_REO_QUEUE_0_UNIFORM_DESCRIPTOR_HEADER_DESCRIPTOR_HEADER_MASK 0xffffffff

/* Description		RX_REO_QUEUE_1_RECEIVE_QUEUE_NUMBER
			
			Indicates the MPDU queue ID to which this MPDU link
			descriptor belongs
			
			Used for tracking and debugging
			
			<legal all>
*/
#define RX_REO_QUEUE_1_RECEIVE_QUEUE_NUMBER_OFFSET                   0x00000004
#define RX_REO_QUEUE_1_RECEIVE_QUEUE_NUMBER_LSB                      0
#define RX_REO_QUEUE_1_RECEIVE_QUEUE_NUMBER_MASK                     0x0000ffff

/* Description		RX_REO_QUEUE_1_RESERVED_1B
			
			<legal 0>
*/
#define RX_REO_QUEUE_1_RESERVED_1B_OFFSET                            0x00000004
#define RX_REO_QUEUE_1_RESERVED_1B_LSB                               16
#define RX_REO_QUEUE_1_RESERVED_1B_MASK                              0xffff0000

/* Description		RX_REO_QUEUE_2_VLD
			
			Valid bit indicating a session is established and the
			queue descriptor is valid(Filled by SW)
			
			<legal all>
*/
#define RX_REO_QUEUE_2_VLD_OFFSET                                    0x00000008
#define RX_REO_QUEUE_2_VLD_LSB                                       0
#define RX_REO_QUEUE_2_VLD_MASK                                      0x00000001

/* Description		RX_REO_QUEUE_2_ASSOCIATED_LINK_DESCRIPTOR_COUNTER
			
			Indicates which of the 3 link descriptor counters shall
			be incremented or decremented when link descriptors are
			added or removed from this flow queue.
			
			MSDU link descriptors related with MPDUs stored in the
			re-order buffer shall also be included in this count.
			
			
			
			<legal 0-2>
*/
#define RX_REO_QUEUE_2_ASSOCIATED_LINK_DESCRIPTOR_COUNTER_OFFSET     0x00000008
#define RX_REO_QUEUE_2_ASSOCIATED_LINK_DESCRIPTOR_COUNTER_LSB        1
#define RX_REO_QUEUE_2_ASSOCIATED_LINK_DESCRIPTOR_COUNTER_MASK       0x00000006

/* Description		RX_REO_QUEUE_2_DISABLE_DUPLICATE_DETECTION
			
			When set, do not perform any duplicate detection.
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_2_DISABLE_DUPLICATE_DETECTION_OFFSET            0x00000008
#define RX_REO_QUEUE_2_DISABLE_DUPLICATE_DETECTION_LSB               3
#define RX_REO_QUEUE_2_DISABLE_DUPLICATE_DETECTION_MASK              0x00000008

/* Description		RX_REO_QUEUE_2_SOFT_REORDER_ENABLE
			
			When set, REO has been instructed to not perform the
			actual re-ordering of frames for this queue, but just to
			insert the reorder opcodes.
			
			
			
			Note that this implies that REO is also not going to
			perform any MSDU level operations, and the entire MPDU (and
			thus pointer to the MSDU link descriptor) will be pushed to
			a destination ring that SW has programmed in a SW
			programmable configuration register in REO
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_2_SOFT_REORDER_ENABLE_OFFSET                    0x00000008
#define RX_REO_QUEUE_2_SOFT_REORDER_ENABLE_LSB                       4
#define RX_REO_QUEUE_2_SOFT_REORDER_ENABLE_MASK                      0x00000010

/* Description		RX_REO_QUEUE_2_AC
			
			Indicates which access category the queue descriptor
			belongs to(filled by SW)
			
			<legal all>
*/
#define RX_REO_QUEUE_2_AC_OFFSET                                     0x00000008
#define RX_REO_QUEUE_2_AC_LSB                                        5
#define RX_REO_QUEUE_2_AC_MASK                                       0x00000060

/* Description		RX_REO_QUEUE_2_BAR
			
			Indicates if  BAR has been received (mostly used for
			debug purpose and this is filled by REO)
			
			<legal all>
*/
#define RX_REO_QUEUE_2_BAR_OFFSET                                    0x00000008
#define RX_REO_QUEUE_2_BAR_LSB                                       7
#define RX_REO_QUEUE_2_BAR_MASK                                      0x00000080

/* Description		RX_REO_QUEUE_2_RTY
			
			Retry bit is checked if this bit is set.  
			
			<legal all>
*/
#define RX_REO_QUEUE_2_RTY_OFFSET                                    0x00000008
#define RX_REO_QUEUE_2_RTY_LSB                                       8
#define RX_REO_QUEUE_2_RTY_MASK                                      0x00000100

/* Description		RX_REO_QUEUE_2_CHK_2K_MODE
			
			Indicates what type of operation is expected from Reo
			when the received frame SN falls within the 2K window
			
			
			
			See REO MLD document for programming details.
			
			<legal all>
*/
#define RX_REO_QUEUE_2_CHK_2K_MODE_OFFSET                            0x00000008
#define RX_REO_QUEUE_2_CHK_2K_MODE_LSB                               9
#define RX_REO_QUEUE_2_CHK_2K_MODE_MASK                              0x00000200

/* Description		RX_REO_QUEUE_2_OOR_MODE
			
			Out of Order mode:
			
			Indicates what type of operation is expected when the
			received frame falls within the OOR window.
			
			
			
			See REO MLD document for programming details.
			
			<legal all>
*/
#define RX_REO_QUEUE_2_OOR_MODE_OFFSET                               0x00000008
#define RX_REO_QUEUE_2_OOR_MODE_LSB                                  10
#define RX_REO_QUEUE_2_OOR_MODE_MASK                                 0x00000400

/* Description		RX_REO_QUEUE_2_BA_WINDOW_SIZE
			
			Indicates the negotiated (window size + 1). 
			
			it can go up to Max of 256bits.
			
			
			
			A value 255 means 256 bitmap, 63 means 64 bitmap, 0
			(means non-BA session, with window size of 0). The 3 values
			here are the main values validated, but other values should
			work as well.
			
			
			
			A BA window size of 0 (=> one frame entry bitmat), means
			that there is NO RX_REO_QUEUE_EXT descriptor following this
			RX_REO_QUEUE STRUCT in memory
			
			
			
			A BA window size of 1 - 105, means that there is 1
			RX_REO_QUEUE_EXT descriptor directly following this
			RX_REO_QUEUE STRUCT in memory.
			
			
			
			A BA window size of 106 - 210, means that there are 2
			RX_REO_QUEUE_EXT descriptors directly following this
			RX_REO_QUEUE STRUCT in memory
			
			
			
			A BA window size of 211 - 256, means that there are 3
			RX_REO_QUEUE_EXT descriptors directly following this
			RX_REO_QUEUE STRUCT in memory
			
			
			
			<legal 0 - 255>
*/
#define RX_REO_QUEUE_2_BA_WINDOW_SIZE_OFFSET                         0x00000008
#define RX_REO_QUEUE_2_BA_WINDOW_SIZE_LSB                            11
#define RX_REO_QUEUE_2_BA_WINDOW_SIZE_MASK                           0x0007f800

/* Description		RX_REO_QUEUE_2_PN_CHECK_NEEDED
			
			When set, REO shall perform the PN increment check
			
			<legal all>
*/
#define RX_REO_QUEUE_2_PN_CHECK_NEEDED_OFFSET                        0x00000008
#define RX_REO_QUEUE_2_PN_CHECK_NEEDED_LSB                           19
#define RX_REO_QUEUE_2_PN_CHECK_NEEDED_MASK                          0x00080000

/* Description		RX_REO_QUEUE_2_PN_SHALL_BE_EVEN
			
			Field only valid when 'pn_check_needed' is set.
			
			
			
			When set, REO shall confirm that the received PN number
			is not only incremented, but also always an even number
			
			<legal all>
*/
#define RX_REO_QUEUE_2_PN_SHALL_BE_EVEN_OFFSET                       0x00000008
#define RX_REO_QUEUE_2_PN_SHALL_BE_EVEN_LSB                          20
#define RX_REO_QUEUE_2_PN_SHALL_BE_EVEN_MASK                         0x00100000

/* Description		RX_REO_QUEUE_2_PN_SHALL_BE_UNEVEN
			
			Field only valid when 'pn_check_needed' is set.
			
			
			
			When set, REO shall confirm that the received PN number
			is not only incremented, but also always an uneven number
			
			<legal all>
*/
#define RX_REO_QUEUE_2_PN_SHALL_BE_UNEVEN_OFFSET                     0x00000008
#define RX_REO_QUEUE_2_PN_SHALL_BE_UNEVEN_LSB                        21
#define RX_REO_QUEUE_2_PN_SHALL_BE_UNEVEN_MASK                       0x00200000

/* Description		RX_REO_QUEUE_2_PN_HANDLING_ENABLE
			
			Field only valid when 'pn_check_needed' is set.
			
			
			
			When set, and REO detected a PN error, HW shall set the
			'pn_error_detected_flag'.
			
			<legal all>
*/
#define RX_REO_QUEUE_2_PN_HANDLING_ENABLE_OFFSET                     0x00000008
#define RX_REO_QUEUE_2_PN_HANDLING_ENABLE_LSB                        22
#define RX_REO_QUEUE_2_PN_HANDLING_ENABLE_MASK                       0x00400000

/* Description		RX_REO_QUEUE_2_PN_SIZE
			
			Size of the PN field check.
			
			Needed for wrap around handling...
			
			
			
			<enum 0     pn_size_24>
			
			<enum 1     pn_size_48>
			
			<enum 2     pn_size_128>
			
			
			
			<legal 0-2>
*/
#define RX_REO_QUEUE_2_PN_SIZE_OFFSET                                0x00000008
#define RX_REO_QUEUE_2_PN_SIZE_LSB                                   23
#define RX_REO_QUEUE_2_PN_SIZE_MASK                                  0x01800000

/* Description		RX_REO_QUEUE_2_IGNORE_AMPDU_FLAG
			
			When set, REO shall ignore the ampdu_flag on the
			entrance descriptor for this queue.
			
			<legal all>
*/
#define RX_REO_QUEUE_2_IGNORE_AMPDU_FLAG_OFFSET                      0x00000008
#define RX_REO_QUEUE_2_IGNORE_AMPDU_FLAG_LSB                         25
#define RX_REO_QUEUE_2_IGNORE_AMPDU_FLAG_MASK                        0x02000000

/* Description		RX_REO_QUEUE_2_RESERVED_2B
			
			<legal 0>
*/
#define RX_REO_QUEUE_2_RESERVED_2B_OFFSET                            0x00000008
#define RX_REO_QUEUE_2_RESERVED_2B_LSB                               26
#define RX_REO_QUEUE_2_RESERVED_2B_MASK                              0xfc000000

/* Description		RX_REO_QUEUE_3_SVLD
			
			Sequence number in next field is valid one. It can be
			filled by SW if the want to fill in the any negotiated SSN,
			otherwise REO will fill the sequence number of first
			received packet and set this bit to 1.
			
			<legal all>
*/
#define RX_REO_QUEUE_3_SVLD_OFFSET                                   0x0000000c
#define RX_REO_QUEUE_3_SVLD_LSB                                      0
#define RX_REO_QUEUE_3_SVLD_MASK                                     0x00000001

/* Description		RX_REO_QUEUE_3_SSN
			
			Starting Sequence number of the session, this changes
			whenever window moves. (can be filled by SW then maintained
			by REO)
			
			<legal all>
*/
#define RX_REO_QUEUE_3_SSN_OFFSET                                    0x0000000c
#define RX_REO_QUEUE_3_SSN_LSB                                       1
#define RX_REO_QUEUE_3_SSN_MASK                                      0x00001ffe

/* Description		RX_REO_QUEUE_3_CURRENT_INDEX
			
			Points to last forwarded packet
			
			<legal all>
*/
#define RX_REO_QUEUE_3_CURRENT_INDEX_OFFSET                          0x0000000c
#define RX_REO_QUEUE_3_CURRENT_INDEX_LSB                             13
#define RX_REO_QUEUE_3_CURRENT_INDEX_MASK                            0x001fe000

/* Description		RX_REO_QUEUE_3_SEQ_2K_ERROR_DETECTED_FLAG
			
			Set by REO, can only be cleared by SW
			
			
			
			When set, REO has detected a 2k error jump in the
			sequence number and from that moment forward, all new frames
			are forwarded directly to FW, without duplicate detect,
			reordering, etc.
			
			<legal all>
*/
#define RX_REO_QUEUE_3_SEQ_2K_ERROR_DETECTED_FLAG_OFFSET             0x0000000c
#define RX_REO_QUEUE_3_SEQ_2K_ERROR_DETECTED_FLAG_LSB                21
#define RX_REO_QUEUE_3_SEQ_2K_ERROR_DETECTED_FLAG_MASK               0x00200000

/* Description		RX_REO_QUEUE_3_PN_ERROR_DETECTED_FLAG
			
			Set by REO, can only be cleared by SW
			
			
			
			When set, REO has detected a PN error and from that
			moment forward, all new frames are forwarded directly to FW,
			without duplicate detect, reordering, etc.
			
			<legal all>
*/
#define RX_REO_QUEUE_3_PN_ERROR_DETECTED_FLAG_OFFSET                 0x0000000c
#define RX_REO_QUEUE_3_PN_ERROR_DETECTED_FLAG_LSB                    22
#define RX_REO_QUEUE_3_PN_ERROR_DETECTED_FLAG_MASK                   0x00400000

/* Description		RX_REO_QUEUE_3_RESERVED_3A
			
			<legal 0>
*/
#define RX_REO_QUEUE_3_RESERVED_3A_OFFSET                            0x0000000c
#define RX_REO_QUEUE_3_RESERVED_3A_LSB                               23
#define RX_REO_QUEUE_3_RESERVED_3A_MASK                              0x7f800000

/* Description		RX_REO_QUEUE_3_PN_VALID
			
			PN number in next fields are valid. It can be filled by
			SW if it wants to fill in the any negotiated SSN, otherwise
			REO will fill the pn based on the first received packet and
			set this bit to 1.
			
			<legal all>
*/
#define RX_REO_QUEUE_3_PN_VALID_OFFSET                               0x0000000c
#define RX_REO_QUEUE_3_PN_VALID_LSB                                  31
#define RX_REO_QUEUE_3_PN_VALID_MASK                                 0x80000000

/* Description		RX_REO_QUEUE_4_PN_31_0
			
			
			<legal all>
*/
#define RX_REO_QUEUE_4_PN_31_0_OFFSET                                0x00000010
#define RX_REO_QUEUE_4_PN_31_0_LSB                                   0
#define RX_REO_QUEUE_4_PN_31_0_MASK                                  0xffffffff

/* Description		RX_REO_QUEUE_5_PN_63_32
			
			Bits [63:32] of the PN number.  
			
			<legal all> 
*/
#define RX_REO_QUEUE_5_PN_63_32_OFFSET                               0x00000014
#define RX_REO_QUEUE_5_PN_63_32_LSB                                  0
#define RX_REO_QUEUE_5_PN_63_32_MASK                                 0xffffffff

/* Description		RX_REO_QUEUE_6_PN_95_64
			
			Bits [95:64] of the PN number.  
			
			<legal all>
*/
#define RX_REO_QUEUE_6_PN_95_64_OFFSET                               0x00000018
#define RX_REO_QUEUE_6_PN_95_64_LSB                                  0
#define RX_REO_QUEUE_6_PN_95_64_MASK                                 0xffffffff

/* Description		RX_REO_QUEUE_7_PN_127_96
			
			Bits [127:96] of the PN number.  
			
			<legal all>
*/
#define RX_REO_QUEUE_7_PN_127_96_OFFSET                              0x0000001c
#define RX_REO_QUEUE_7_PN_127_96_LSB                                 0
#define RX_REO_QUEUE_7_PN_127_96_MASK                                0xffffffff

/* Description		RX_REO_QUEUE_8_LAST_RX_ENQUEUE_TIMESTAMP
			
			This timestamp is updated when an MPDU is received and
			accesses this Queue Descriptor. It does not include the
			access due to Command TLVs or Aging (which will be updated
			in Last_rx_dequeue_timestamp).
			
			<legal all>
*/
#define RX_REO_QUEUE_8_LAST_RX_ENQUEUE_TIMESTAMP_OFFSET              0x00000020
#define RX_REO_QUEUE_8_LAST_RX_ENQUEUE_TIMESTAMP_LSB                 0
#define RX_REO_QUEUE_8_LAST_RX_ENQUEUE_TIMESTAMP_MASK                0xffffffff

/* Description		RX_REO_QUEUE_9_LAST_RX_DEQUEUE_TIMESTAMP
			
			This timestamp is used for Aging. When an MPDU or
			multiple MPDUs are forwarded, either due to window movement,
			bar, aging or command flush, this timestamp is updated. Also
			when the bitmap is all zero and the first time an MPDU is
			queued (opcode=QCUR), this timestamp is updated for aging.
			
			<legal all>
*/
#define RX_REO_QUEUE_9_LAST_RX_DEQUEUE_TIMESTAMP_OFFSET              0x00000024
#define RX_REO_QUEUE_9_LAST_RX_DEQUEUE_TIMESTAMP_LSB                 0
#define RX_REO_QUEUE_9_LAST_RX_DEQUEUE_TIMESTAMP_MASK                0xffffffff

/* Description		RX_REO_QUEUE_10_PTR_TO_NEXT_AGING_QUEUE_31_0
			
			Address  (address bits 31-0)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the last entry in the list.
			
			<legal all>
*/
#define RX_REO_QUEUE_10_PTR_TO_NEXT_AGING_QUEUE_31_0_OFFSET          0x00000028
#define RX_REO_QUEUE_10_PTR_TO_NEXT_AGING_QUEUE_31_0_LSB             0
#define RX_REO_QUEUE_10_PTR_TO_NEXT_AGING_QUEUE_31_0_MASK            0xffffffff

/* Description		RX_REO_QUEUE_11_PTR_TO_NEXT_AGING_QUEUE_39_32
			
			Address  (address bits 39-32)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the last entry in the list.
			
			<legal all>
*/
#define RX_REO_QUEUE_11_PTR_TO_NEXT_AGING_QUEUE_39_32_OFFSET         0x0000002c
#define RX_REO_QUEUE_11_PTR_TO_NEXT_AGING_QUEUE_39_32_LSB            0
#define RX_REO_QUEUE_11_PTR_TO_NEXT_AGING_QUEUE_39_32_MASK           0x000000ff

/* Description		RX_REO_QUEUE_11_RESERVED_11A
			
			<legal 0>
*/
#define RX_REO_QUEUE_11_RESERVED_11A_OFFSET                          0x0000002c
#define RX_REO_QUEUE_11_RESERVED_11A_LSB                             8
#define RX_REO_QUEUE_11_RESERVED_11A_MASK                            0xffffff00

/* Description		RX_REO_QUEUE_12_PTR_TO_PREVIOUS_AGING_QUEUE_31_0
			
			Address  (address bits 31-0)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the first entry in the list.
			
			<legal all>
*/
#define RX_REO_QUEUE_12_PTR_TO_PREVIOUS_AGING_QUEUE_31_0_OFFSET      0x00000030
#define RX_REO_QUEUE_12_PTR_TO_PREVIOUS_AGING_QUEUE_31_0_LSB         0
#define RX_REO_QUEUE_12_PTR_TO_PREVIOUS_AGING_QUEUE_31_0_MASK        0xffffffff

/* Description		RX_REO_QUEUE_13_PTR_TO_PREVIOUS_AGING_QUEUE_39_32
			
			Address  (address bits 39-32)of next RX_REO_QUEUE
			descriptor in the 'receive timestamp' ordered list.
			
			From it the Position of this queue descriptor in the per
			AC aging waitlist  can be derived.
			
			Value 0x0 indicates the 'NULL' pointer which implies
			that this is the first entry in the list.
			
			<legal all>
*/
#define RX_REO_QUEUE_13_PTR_TO_PREVIOUS_AGING_QUEUE_39_32_OFFSET     0x00000034
#define RX_REO_QUEUE_13_PTR_TO_PREVIOUS_AGING_QUEUE_39_32_LSB        0
#define RX_REO_QUEUE_13_PTR_TO_PREVIOUS_AGING_QUEUE_39_32_MASK       0x000000ff

/* Description		RX_REO_QUEUE_13_RESERVED_13A
			
			<legal 0>
*/
#define RX_REO_QUEUE_13_RESERVED_13A_OFFSET                          0x00000034
#define RX_REO_QUEUE_13_RESERVED_13A_LSB                             8
#define RX_REO_QUEUE_13_RESERVED_13A_MASK                            0xffffff00

/* Description		RX_REO_QUEUE_14_RX_BITMAP_31_0
			
			When a bit is set, the corresponding frame is currently
			held in the re-order queue.
			
			The bitmap  is Fully managed by HW. 
			
			SW shall init this to 0, and then never ever change it
			
			<legal all>
*/
#define RX_REO_QUEUE_14_RX_BITMAP_31_0_OFFSET                        0x00000038
#define RX_REO_QUEUE_14_RX_BITMAP_31_0_LSB                           0
#define RX_REO_QUEUE_14_RX_BITMAP_31_0_MASK                          0xffffffff

/* Description		RX_REO_QUEUE_15_RX_BITMAP_63_32
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_15_RX_BITMAP_63_32_OFFSET                       0x0000003c
#define RX_REO_QUEUE_15_RX_BITMAP_63_32_LSB                          0
#define RX_REO_QUEUE_15_RX_BITMAP_63_32_MASK                         0xffffffff

/* Description		RX_REO_QUEUE_16_RX_BITMAP_95_64
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_16_RX_BITMAP_95_64_OFFSET                       0x00000040
#define RX_REO_QUEUE_16_RX_BITMAP_95_64_LSB                          0
#define RX_REO_QUEUE_16_RX_BITMAP_95_64_MASK                         0xffffffff

/* Description		RX_REO_QUEUE_17_RX_BITMAP_127_96
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_17_RX_BITMAP_127_96_OFFSET                      0x00000044
#define RX_REO_QUEUE_17_RX_BITMAP_127_96_LSB                         0
#define RX_REO_QUEUE_17_RX_BITMAP_127_96_MASK                        0xffffffff

/* Description		RX_REO_QUEUE_18_RX_BITMAP_159_128
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_18_RX_BITMAP_159_128_OFFSET                     0x00000048
#define RX_REO_QUEUE_18_RX_BITMAP_159_128_LSB                        0
#define RX_REO_QUEUE_18_RX_BITMAP_159_128_MASK                       0xffffffff

/* Description		RX_REO_QUEUE_19_RX_BITMAP_191_160
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_19_RX_BITMAP_191_160_OFFSET                     0x0000004c
#define RX_REO_QUEUE_19_RX_BITMAP_191_160_LSB                        0
#define RX_REO_QUEUE_19_RX_BITMAP_191_160_MASK                       0xffffffff

/* Description		RX_REO_QUEUE_20_RX_BITMAP_223_192
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_20_RX_BITMAP_223_192_OFFSET                     0x00000050
#define RX_REO_QUEUE_20_RX_BITMAP_223_192_LSB                        0
#define RX_REO_QUEUE_20_RX_BITMAP_223_192_MASK                       0xffffffff

/* Description		RX_REO_QUEUE_21_RX_BITMAP_255_224
			
			See Rx_bitmap_31_0 description
			
			<legal all>
*/
#define RX_REO_QUEUE_21_RX_BITMAP_255_224_OFFSET                     0x00000054
#define RX_REO_QUEUE_21_RX_BITMAP_255_224_LSB                        0
#define RX_REO_QUEUE_21_RX_BITMAP_255_224_MASK                       0xffffffff

/* Description		RX_REO_QUEUE_22_CURRENT_MPDU_COUNT
			
			The number of MPDUs in the queue.
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_22_CURRENT_MPDU_COUNT_OFFSET                    0x00000058
#define RX_REO_QUEUE_22_CURRENT_MPDU_COUNT_LSB                       0
#define RX_REO_QUEUE_22_CURRENT_MPDU_COUNT_MASK                      0x0000007f

/* Description		RX_REO_QUEUE_22_CURRENT_MSDU_COUNT
			
			The number of MSDUs in the queue.
			
			<legal all>
*/
#define RX_REO_QUEUE_22_CURRENT_MSDU_COUNT_OFFSET                    0x00000058
#define RX_REO_QUEUE_22_CURRENT_MSDU_COUNT_LSB                       7
#define RX_REO_QUEUE_22_CURRENT_MSDU_COUNT_MASK                      0xffffff80

/* Description		RX_REO_QUEUE_23_RESERVED_23
			
			<legal 0>
*/
#define RX_REO_QUEUE_23_RESERVED_23_OFFSET                           0x0000005c
#define RX_REO_QUEUE_23_RESERVED_23_LSB                              0
#define RX_REO_QUEUE_23_RESERVED_23_MASK                             0x0000000f

/* Description		RX_REO_QUEUE_23_TIMEOUT_COUNT
			
			The number of times that REO started forwarding frames
			even though there is a hole in the bitmap. Forwarding reason
			is Timeout
			
			
			
			The counter saturates and freezes at 0x3F
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_23_TIMEOUT_COUNT_OFFSET                         0x0000005c
#define RX_REO_QUEUE_23_TIMEOUT_COUNT_LSB                            4
#define RX_REO_QUEUE_23_TIMEOUT_COUNT_MASK                           0x000003f0

/* Description		RX_REO_QUEUE_23_FORWARD_DUE_TO_BAR_COUNT
			
			The number of times that REO started forwarding frames
			even though there is a hole in the bitmap. Forwarding reason
			is reception of BAR frame.
			
			
			
			The counter saturates and freezes at 0x3F
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_23_FORWARD_DUE_TO_BAR_COUNT_OFFSET              0x0000005c
#define RX_REO_QUEUE_23_FORWARD_DUE_TO_BAR_COUNT_LSB                 10
#define RX_REO_QUEUE_23_FORWARD_DUE_TO_BAR_COUNT_MASK                0x0000fc00

/* Description		RX_REO_QUEUE_23_DUPLICATE_COUNT
			
			The number of duplicate frames that have been detected
			
			<legal all>
*/
#define RX_REO_QUEUE_23_DUPLICATE_COUNT_OFFSET                       0x0000005c
#define RX_REO_QUEUE_23_DUPLICATE_COUNT_LSB                          16
#define RX_REO_QUEUE_23_DUPLICATE_COUNT_MASK                         0xffff0000

/* Description		RX_REO_QUEUE_24_FRAMES_IN_ORDER_COUNT
			
			The number of frames that have been received in order
			(without a hole that prevented them from being forwarded
			immediately)
			
			
			
			This corresponds to the Reorder opcodes:
			
			'FWDCUR' and 'FWD BUF'
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_24_FRAMES_IN_ORDER_COUNT_OFFSET                 0x00000060
#define RX_REO_QUEUE_24_FRAMES_IN_ORDER_COUNT_LSB                    0
#define RX_REO_QUEUE_24_FRAMES_IN_ORDER_COUNT_MASK                   0x00ffffff

/* Description		RX_REO_QUEUE_24_BAR_RECEIVED_COUNT
			
			The number of times a BAR frame is received.
			
			
			
			This corresponds to the Reorder opcodes with 'DROP'
			
			
			
			The counter saturates and freezes at 0xFF
			
			<legal all>
*/
#define RX_REO_QUEUE_24_BAR_RECEIVED_COUNT_OFFSET                    0x00000060
#define RX_REO_QUEUE_24_BAR_RECEIVED_COUNT_LSB                       24
#define RX_REO_QUEUE_24_BAR_RECEIVED_COUNT_MASK                      0xff000000

/* Description		RX_REO_QUEUE_25_MPDU_FRAMES_PROCESSED_COUNT
			
			The total number of MPDU frames that have been processed
			by REO. 'Processing' here means that REO has received them
			out of the entrance ring, and retrieved the corresponding
			RX_REO_QUEUE Descriptor. 
			
			
			
			Note that this count includes duplicates, frames that
			later had errors, etc.
			
			
			
			Note that field 'Duplicate_count' indicates how many of
			these MPDUs were duplicates.
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_25_MPDU_FRAMES_PROCESSED_COUNT_OFFSET           0x00000064
#define RX_REO_QUEUE_25_MPDU_FRAMES_PROCESSED_COUNT_LSB              0
#define RX_REO_QUEUE_25_MPDU_FRAMES_PROCESSED_COUNT_MASK             0xffffffff

/* Description		RX_REO_QUEUE_26_MSDU_FRAMES_PROCESSED_COUNT
			
			The total number of MSDU frames that have been processed
			by REO. 'Processing' here means that REO has received them
			out of the entrance ring, and retrieved the corresponding
			RX_REO_QUEUE Descriptor. 
			
			
			
			Note that this count includes duplicates, frames that
			later had errors, etc.
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_26_MSDU_FRAMES_PROCESSED_COUNT_OFFSET           0x00000068
#define RX_REO_QUEUE_26_MSDU_FRAMES_PROCESSED_COUNT_LSB              0
#define RX_REO_QUEUE_26_MSDU_FRAMES_PROCESSED_COUNT_MASK             0xffffffff

/* Description		RX_REO_QUEUE_27_TOTAL_PROCESSED_BYTE_COUNT
			
			An approximation of the number of bytes processed for
			this queue. 
			
			'Processing' here means that REO has received them out
			of the entrance ring, and retrieved the corresponding
			RX_REO_QUEUE Descriptor. 
			
			
			
			Note that this count includes duplicates, frames that
			later had errors, etc.
			
			
			
			In 64 byte units
			
			<legal all>
*/
#define RX_REO_QUEUE_27_TOTAL_PROCESSED_BYTE_COUNT_OFFSET            0x0000006c
#define RX_REO_QUEUE_27_TOTAL_PROCESSED_BYTE_COUNT_LSB               0
#define RX_REO_QUEUE_27_TOTAL_PROCESSED_BYTE_COUNT_MASK              0xffffffff

/* Description		RX_REO_QUEUE_28_LATE_RECEIVE_MPDU_COUNT
			
			The number of MPDUs received after the window had
			already moved on. The 'late' sequence window is defined as
			(Window SSN - 256) - (Window SSN - 1)
			
			
			
			This corresponds with Out of order detection in
			duplicate detect FSM
			
			
			
			The counter saturates and freezes at 0xFFF
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_28_LATE_RECEIVE_MPDU_COUNT_OFFSET               0x00000070
#define RX_REO_QUEUE_28_LATE_RECEIVE_MPDU_COUNT_LSB                  0
#define RX_REO_QUEUE_28_LATE_RECEIVE_MPDU_COUNT_MASK                 0x00000fff

/* Description		RX_REO_QUEUE_28_WINDOW_JUMP_2K
			
			The number of times the window moved more then 2K
			
			
			
			The counter saturates and freezes at 0xF
			
			
			
			(Note: field name can not start with number: previous
			2k_window_jump)
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_28_WINDOW_JUMP_2K_OFFSET                        0x00000070
#define RX_REO_QUEUE_28_WINDOW_JUMP_2K_LSB                           12
#define RX_REO_QUEUE_28_WINDOW_JUMP_2K_MASK                          0x0000f000

/* Description		RX_REO_QUEUE_28_HOLE_COUNT
			
			The number of times a hole was created in the receive
			bitmap.
			
			
			
			This corresponds to the Reorder opcodes with 'QCUR'
			
			
			
			<legal all>
*/
#define RX_REO_QUEUE_28_HOLE_COUNT_OFFSET                            0x00000070
#define RX_REO_QUEUE_28_HOLE_COUNT_LSB                               16
#define RX_REO_QUEUE_28_HOLE_COUNT_MASK                              0xffff0000

/* Description		RX_REO_QUEUE_29_RESERVED_29
			
			<legal 0>
*/
#define RX_REO_QUEUE_29_RESERVED_29_OFFSET                           0x00000074
#define RX_REO_QUEUE_29_RESERVED_29_LSB                              0
#define RX_REO_QUEUE_29_RESERVED_29_MASK                             0xffffffff

/* Description		RX_REO_QUEUE_30_RESERVED_30
			
			<legal 0>
*/
#define RX_REO_QUEUE_30_RESERVED_30_OFFSET                           0x00000078
#define RX_REO_QUEUE_30_RESERVED_30_LSB                              0
#define RX_REO_QUEUE_30_RESERVED_30_MASK                             0xffffffff

/* Description		RX_REO_QUEUE_31_RESERVED_31
			
			<legal 0>
*/
#define RX_REO_QUEUE_31_RESERVED_31_OFFSET                           0x0000007c
#define RX_REO_QUEUE_31_RESERVED_31_LSB                              0
#define RX_REO_QUEUE_31_RESERVED_31_MASK                             0xffffffff


#endif // _RX_REO_QUEUE_H_