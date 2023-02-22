#pragma once

#include "bitfield.h"
#include "bitfield_r.h"
#include "device_register.h"
#include <cstdint>

namespace L3GD20 {

	constexpr uint8_t address = 0xD2;

	class CTRL_REG1 : public DeviceRegister<0x20, 1> {
		public:
			struct DR_BW {
				enum Enum {
					OUTPUT_DATA_RATE_95HZ_CUTOFF_12_5 = 0,
					OUTPUT_DATA_RATE_95HZ_CUTOFF_25 = 1,
					OUTPUT_DATA_RATE_190HZ_CUTOFF_12_5 = 4,
					OUTPUT_DATA_RATE_190HZ_CUTOFF_25 = 5,
					OUTPUT_DATA_RATE_190HZ_CUTOFF_50 = 6,
					OUTPUT_DATA_RATE_190HZ_CUTOFF_70 = 7,
					OUTPUT_DATA_RATE_380HZ_CUTOFF_20 = 8,
					OUTPUT_DATA_RATE_380HZ_CUTOFF_25 = 9,
					OUTPUT_DATA_RATE_380HZ_CUTOFF_50 = 10,
					OUTPUT_DATA_RATE_380HZ_CUTOFF_100 = 11,
					OUTPUT_DATA_RATE_760HZ_CUTOFF_30 = 12,
					OUTPUT_DATA_RATE_760HZ_CUTOFF_35 = 13,
					OUTPUT_DATA_RATE_760HZ_CUTOFF_50 = 14,
					OUTPUT_DATA_RATE_760HZ_CUTOFF_100 = 15
				};
				using Bits = BitField<uint8_t, 4, 4, Enum>;
			};

			struct PD {
				enum Enum {
					POWER_DOWN = 0,
					NORMAL_MODE_OR_SLEEP_MODE = 1
				};
				using Bits = BitField<uint8_t, 3, 1, Enum>;
			};

			struct Zen {
				enum Enum {
					Z_AXIS_DISABLED = 0,
					Z_AXIS_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 2, 1, Enum>;
			};
			
			struct Xen {
				enum Enum {
					X_AXIS_DISABLED = 0,
					X_AXIS_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 1, 1, Enum>;
			};

			struct Yen {
				enum Enum {
					Y_AXIS_DISABLED = 0,
					Y_AXIS_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 0, 1, Enum>;
			};
		
			union {
				union {
					DR_BW::Bits dr_bw;
					PD::Bits pd;
					Zen::Bits zen;
					Yen::Bits yen;
					Xen::Bits xen;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG2 : public DeviceRegister<0x21, 1> {
		public:
			struct HPM {
				enum Enum {
					NORMAL_MODE_RESET_READING_HP_RESET_FILTER = 0,
					REFERENCE_SIGNAL_FOR_FILTERING = 1,
					NORMAL_MODE = 2,
					AUTORESET_ON_INTERRUPT_EVENT = 3
				};
				using Bits = BitField<uint8_t, 4, 2, Enum>;
			};

			struct HPCF {
				enum Enum {
					_0000 = 0,
					_0001 = 0,
					_0010 = 0,
					_0011 = 0,
					_0100 = 0,
					_0101 = 0,
					_0110 = 0,
					_0111 = 0,
					_1000 = 0,
					_1001 = 0
				};
				using Bits = BitField<uint8_t, 0, 4, Enum>;
			};
		
			union {
				union {
					HPM::Bits hpm;
					HPCF::Bits hpcf;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG3 : public DeviceRegister<0x22, 1> {
		public:
			struct Ix {
				enum Enum {
					DISABLE = 0,
					ENABLE = 1
				};
			};

			struct H_LACTIVE {
				enum Enum {
					HIGH = 0,
					LOW = 1
				};
				using Bits = BitField<uint8_t, 5, 1, Enum>;
			};

			struct PP_OD {
				enum Enum {
					PUSH_PULL = 0,
					OPEN_DRAIN = 1
				};
				using Bits = BitField<uint8_t, 4, 1, Enum>;
			};

		private:
			template<int X>
			struct Ix_Base {
				using Bits = BitField<uint8_t, X, 1, Ix::Enum>;
			};
		
		public:
			union {
				union {
					Ix_Base<7>::Bits i1_int1;
					Ix_Base<6>::Bits i1_boot;
					H_LACTIVE::Bits h_lactive;
					PP_OD::Bits pp_od;
					Ix_Base<3>::Bits i2_drdy;
					Ix_Base<2>::Bits i2_wtm;
					Ix_Base<1>::Bits i2_orun;
					Ix_Base<0>::Bits i2_empty;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG4 : public DeviceRegister<0x23, 1>{
		public:
			struct BDU {
				enum Enum {
					CONTINOUS_UPDATE = 0,
					OUTPUT_REGISTERS_NOT_UPDATED_UNTIL_MSB_AND_LSB_HAVE_BEEN_READ = 1
				};
				using Bits = BitField<uint8_t, 7, 1, Enum>;
			};

			struct BLE {
				enum Enum {
					DATA_LSB_AT_LOWER_ADDRESS = 0,
					DATA_MSB_AT_LOWER_ADDRESS = 1
				};
				using Bits = BitField<uint8_t, 6, 1, Enum>;
			};

			struct FS {
				enum Enum {
					_250DPS = 0,
					_500DPS = 1,
					_2000DPS = 2
				};
				using Bits = BitField<uint8_t, 4, 2, Enum>;
			};

			struct SIM {
				enum Enum {
					_4_WIRE_INTERFACE = 0,
					_3_WIRE_INTERFACE = 1
				};
				using Bits = BitField<uint8_t, 0, 1, Enum>;
			};
		
			union {
				union {
					BDU::Bits bdu;
					BLE::Bits ble;
					FS::Bits fs;
					SIM::Bits sim;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG5 : public DeviceRegister<0x24, 1>{
		public:
			struct BOOT {
				enum Enum {
					NORMAL_MODE = 0,
					REBOOT_MEMORY_CONTENT = 1
				};
				using Bits = BitField<uint8_t, 7, 1, Enum>;
			};

			struct FIFO_EN {
				enum Enum {
					FIFO_DISABLE = 0,
					FIFO_ENABLE = 1
				};
				using Bits = BitField<uint8_t, 6, 1, Enum>;
			};

			struct HPen {
				enum Enum {
					HPF_DISABLED = 0,
					HPF_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 4, 1, Enum>;
			};

			struct INT1_Sel {
				enum Enum {
					_00 = 0,
					_01 = 1,
					_10 = 2,
					_11 = 3
				};
				using Bits = BitField<uint8_t, 2, 2, Enum>;
			};

			struct Out_Sel {
				enum Enum {
					_00 = 0,
					_01 = 1,
					_10 = 2,
					_11 = 3
				};
				using Bits = BitField<uint8_t, 0, 2, Enum>;
			};

		public:
			union {
				union {
					BOOT::Bits boot;
					FIFO_EN::Bits fifo_en;
					HPen::Bits hpen;
					INT1_Sel::Bits int1_sel;
					Out_Sel::Bits out_sel;
				};
				uint8_t bits;
			};
	};

	class REFERENCE : public DeviceRegister<0x25, 1>{
		public:
			struct Ref {
				using Bits = BitField<uint8_t, 0, 8, uint8_t>;
			};

			union {
				union {
					Ref::Bits ref;
				};
				uint8_t bits;
			};
	};

	class OUT_TEMP : public DeviceRegister<0x26, 1> {
		public:
			struct DATA {
				using Bits = BitFieldReadOnly<uint8_t, 0, 8, int8_t>;
			};
		
			union {
				union {
					DATA::Bits data;
				};
				uint8_t bits;
			};
	};

	class STATUS_REG : public DeviceRegister<0x27, 1> {
		public:
			struct xOR {
				enum Enum {
					NO_OVERRUN_HAS_OCCURRED = 0,
					NEW_DATA_HAS_OVERWRITTEN_THE_PREVIOUS_DATA = 1
				};
			};

			struct xDA {
				enum Enum {
					NEW_DATA_IS_NOT_YET_AVAIABLE = 0,
					NEW_DATA_IS_AVAIABLE = 1
				};
			};
		
		private:
			template<int X>
			struct xOR_Base {
				using Bits = BitFieldReadOnly<uint8_t, X, 1, xOR::Enum>;
			};

			template<int X>
			struct xDA_Base {
				using Bits = BitFieldReadOnly<uint8_t, X, 1, xDA::Enum>;
			};

		public:
			union {
				union {
					xOR_Base<7>::Bits zyxor;
					xOR_Base<6>::Bits zor;
					xOR_Base<5>::Bits yor;
					xOR_Base<4>::Bits _xor;
					xDA_Base<3>::Bits zyxda;
					xDA_Base<2>::Bits zda;
					xDA_Base<1>::Bits yda;
					xDA_Base<0>::Bits xda;
				};
				uint8_t bits;
			};
	};

	class OUT_X : public DeviceRegister<0x28 | 0x80, 2> {
		public:
			struct DATA {
				using Bits = BitFieldReadOnly<uint16_t, 0, 16, int16_t>;
			};
		
			union {
				union {
					DATA::Bits data;
				};
				uint16_t bits;
			};
	};

	class OUT_Y : public DeviceRegister<0x2A | 0x80, 2> {
		public:
			struct DATA {
				using Bits = BitFieldReadOnly<uint16_t, 0, 16, int16_t>;
			};
		
			union {
				union {
					DATA::Bits data;
				};
				uint16_t bits;
			};
	};

	class OUT_Z : public DeviceRegister<0x2C | 0x80, 2> {
		public:
			struct DATA {
				using Bits = BitFieldReadOnly<uint16_t, 0, 16, int16_t>;
			};
		
			union {
				union {
					DATA::Bits data;
				};
				uint16_t bits;
			};
	};

}
