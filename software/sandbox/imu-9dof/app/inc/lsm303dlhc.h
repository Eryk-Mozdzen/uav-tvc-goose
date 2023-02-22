#pragma once

#pragma once

#include "bitfield.h"
#include "bitfield_r.h"
#include "device_register.h"
#include <cstdint>

namespace LSM303DLHC {

	constexpr uint8_t acc_address = 0x32;
	constexpr uint8_t mag_address = 0x3C;

	class CTRL_REG1_A : public DeviceRegister<0x20, 1> {
		public:

			struct ODR {
				enum Enum {
					POWER_DOWN = 0,
					NORMAL_LOW_POWER_1HZ = 1,
					NORMAL_LOW_POWER_10HZ = 2,
					NORMAL_LOW_POWER_25HZ = 3,
					NORMAL_LOW_POWER_50HZ = 4,
					NORMAL_LOW_POWER_100HZ = 5,
					NORMAL_LOW_POWER_200HZ = 6,
					NORMAL_LOW_POWER_400HZ = 7,
					LOW_POWER_1620HZ = 8,
					NORMAL_1344HZ_LOW_POWER_5376HZ = 9
				};
				using Bits = BitField<uint8_t, 4, 4, Enum>;
			};

			struct LPen {
				enum Enum {
					NORMAL_MODE = 0,
					LOW_POWER_MODE = 1
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

			struct Yen {
				enum Enum {
					Y_AXIS_DISABLED = 0,
					Y_AXIS_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 1, 1, Enum>;
			};
			
			struct Xen {
				enum Enum {
					X_AXIS_DISABLED = 0,
					X_AXIS_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 0, 1, Enum>;
			};
		
			union {
				union {
					ODR::Bits odr;
					LPen::Bits lpen;
					Zen::Bits zen;
					Yen::Bits yen;
					Xen::Bits xen;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG2_A : public DeviceRegister<0x21, 1> {
		public:

			struct HPx {
				enum Enum {
					FILTER_BYPASSED = 0,
					FILTER_ENABLED = 1
				};
			};

			struct FDS {
				enum Enum {
					INTERNAL_FILTER_BYPASSED = 0,
					DATA_FROM_INTERNAL_FILTER_SENT_TO_OUTPUT_REGISTER_AND_FIFO = 1
				};
				using Bits = BitField<uint8_t, 3, 1, Enum>;
			};

			struct HPCF {
				enum Enum {

				};
				using Bits = BitField<uint8_t, 4, 2, Enum>;
			};

			struct HPM {
				enum Enum {
					NORMAL_MODE_RESET_READING_HP_RESET_FILTER = 0,
					REFERENCE_SIGNAL_FOR_FILTERING = 1,
					NORMAL_MODE = 2,
					AUTORESET_ON_INTERRUPT_EVENT = 3
				};
				using Bits = BitField<uint8_t, 6, 2, Enum>;
			};

		private:
			template<int X>
			struct HPx_Base {
				using Bits = BitField<uint8_t, X, 1, HPx::Enum>;
			};
		public:
		
			union {
				union {
					HPM::Bits hpm;
					HPCF::Bits hpcf;
					FDS::Bits fds;
					HPx_Base<2>::Bits hpclick;
					HPx_Base<1>::Bits hpis1;
					HPx_Base<0>::Bits hpis0;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG3_A : public DeviceRegister<0x22, 1>{
		public:
			struct I1x {
				enum Enum {
					DISABLE = 0,
					ENABLE = 1
				};
			};

		private:
			template<int X>
			struct I1x_Base {
				using Bits = BitField<uint8_t, X, 1, I1x::Enum>;
			};

		public:
		
			union {
				union {
					I1x_Base<7>::Bits i1_click;
					I1x_Base<6>::Bits i1_aoi1;
					I1x_Base<5>::Bits i1_aoi2;
					I1x_Base<4>::Bits i1_drdy1;
					I1x_Base<3>::Bits i1_drdy2;
					I1x_Base<2>::Bits i1_wtm;
					I1x_Base<1>::Bits i1_overrun;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG4_A : public DeviceRegister<0x23, 1>{
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
					_2G = 0,
					_4G = 1,
					_8G = 2,
					_16G = 3
				};
				using Bits = BitField<uint8_t, 4, 2, Enum>;
			};

			struct HR {
				enum Enum {
					HIGH_RESOLUTION_DISABLED = 0,
					HIGH_RESOLUTION_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 3, 1, Enum>;
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
					HR::Bits hr;
					SIM::Bits sim;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG5_A : public DeviceRegister<0x24, 1>{
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

			struct LIR_INTx {
				enum Enum {
					INTERRUPT_REQUEST_NOT_LATCHED = 0,
					INTERRUPT_REQUEST_LATCHED = 1
				};
			};

			struct D4D_INTx {
				enum Enum {
					DISABLED = 0,
					ENABLED = 1
				};
			};

		private:
			template<int X>
			struct LIR_INTx_Base {
				using Bits = BitField<uint8_t, X, 1, LIR_INTx::Enum>;
			};

			template<int X>
			struct D4D_INTx_Base {
				using Bits = BitField<uint8_t, X, 1, D4D_INTx::Enum>;
			};

		public:
			union {
				union {
					BOOT::Bits boot;
					FIFO_EN::Bits fifo_en;
					LIR_INTx_Base<3>::Bits lir_int1;
					D4D_INTx_Base<2>::Bits d4d_int1;
					LIR_INTx_Base<1>::Bits lir_int2;
					D4D_INTx_Base<0>::Bits d4d_int2;
				};
				uint8_t bits;
			};
	};

	class CTRL_REG6_A : public DeviceRegister<0x25, 1>{
		public:
			struct BOOT_I1 {
				enum Enum {
					DISABLE = 0,
					ENABLE = 1
				};
				using Bits = BitField<uint8_t, 4, 1, Enum>;
			};

			struct P2_ACT {
				enum Enum {
					DISABLE = 0,
					ENABLE = 1
				};
				using Bits = BitField<uint8_t, 3, 1, Enum>;
			};

			struct H_LACTIVE {
				enum Enum {
					ACTIVE_HIGH = 0,
					ACTIVE_LOW = 1
				};
				using Bits = BitField<uint8_t, 1, 1, Enum>;
			};

			struct I2x {
				enum Enum {
					DISABLE = 0,
					ENABLE = 1
				};
			};

		private:
			template<int X>
			struct I2x_Base {
				using Bits = BitField<uint8_t, X, 1, I2x::Enum>;
			};

		public:
			union {
				union {
					I2x_Base<7>::Bits i2_clicken;
					I2x_Base<6>::Bits i2_int1;
					I2x_Base<5>::Bits i2_int2;
					BOOT_I1::Bits boot_i1;
					P2_ACT::Bits p22_act;
					H_LACTIVE::Bits h_lactive;
				};
				uint8_t bits;
			};
	};

	class REFERENCE_A : public DeviceRegister<0x26, 1>{
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

	class STATUS_REG_A : public DeviceRegister<0x27, 1> {
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

	class OUT_X_A : public DeviceRegister<0x28 | 0x80, 2> {
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

	class OUT_Y_A : public DeviceRegister<0x2A | 0x80, 2> {
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

	class OUT_Z_A : public DeviceRegister<0x2C | 0x80, 2> {
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

	class CRA_REG_M : public DeviceRegister<0x00, 1>{
		public:
			struct TEMP_EN {
				enum Enum {
					TEMPERATURE_SENSOR_DISABLED = 0,
					TEMPERATURE_SENSOR_ENABLED = 1
				};
				using Bits = BitField<uint8_t, 7, 1, Enum>;	
			};

			struct DO {
				enum Enum {
					DATA_OUTPUT_RATE_0_75HZ = 0,
					DATA_OUTPUT_RATE_1_5HZ = 1,
					DATA_OUTPUT_RATE_3HZ = 2,
					DATA_OUTPUT_RATE_7_5HZ = 3,
					DATA_OUTPUT_RATE_15HZ = 4,
					DATA_OUTPUT_RATE_30HZ = 5,
					DATA_OUTPUT_RATE_75HZ = 6,
					DATA_OUTPUT_RATE_220HZ = 7
				};
				using Bits = BitField<uint8_t, 2, 3, Enum>;	
			};
		
			union {
				union {
					TEMP_EN::Bits temp_en;
					DO::Bits _do;
				};
				uint8_t bits;
			};
	};

	class CRB_REG_M : public DeviceRegister<0x01, 1>{
		public:
			struct GN {
				enum Enum {
					SENSOR_INPUT_FIELD_RANGE_1_3GAUSS = 1,
					SENSOR_INPUT_FIELD_RANGE_1_9GAUSS = 2,
					SENSOR_INPUT_FIELD_RANGE_2_5GAUSS = 3,
					SENSOR_INPUT_FIELD_RANGE_4GAUSS = 4,
					SENSOR_INPUT_FIELD_RANGE_4_7GAUSS = 5,
					SENSOR_INPUT_FIELD_RANGE_5_6GAUSS = 6,
					SENSOR_INPUT_FIELD_RANGE_8_1GAUSS = 7
				};
				using Bits = BitField<uint8_t, 5, 3, Enum>;	
			};
		
			union {
				union {
					GN::Bits gn;
				};
				uint8_t bits;
			};
	};

	class MR_REG_M : public DeviceRegister<0x02, 1>{
		public:
			struct MD {
				enum Enum {
					CONTINOUS_CONVERSION_MODE = 0,
					SINGLE_CONVERSION_MODE = 1,
					SLEEP_MODE = 2
				};
				using Bits = BitField<uint8_t, 0, 2, Enum>;	
			};
		
			union {
				union {
					MD::Bits md;
				};
				uint8_t bits;
			};
	};

	class OUT_X_M : public DeviceRegister<0x03, 2> {
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

	class OUT_Y_M : public DeviceRegister<0x05, 2> {
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

	class OUT_Z_M : public DeviceRegister<0x07, 2> {
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

	class SR_REG_M : public DeviceRegister<0x09, 1> {
		public:
			struct LOCK {
				enum Enum {
					DATA_OUTPUT_REGISTER_UNLOCK = 0,
					DATA_OUTPUT_REGISTER_LOCK = 1
				};
				using Bits = BitFieldReadOnly<uint16_t, 1, 1, Enum>;
			};

			struct DRDY {
				enum Enum {
					NEW_SET_OF_MEASURMENTS_ARE_NOT_READY = 0,
					NEW_SET_OF_MEASURMENTS_ARE_READY = 1
				};
				using Bits = BitFieldReadOnly<uint16_t, 0, 1, Enum>;
			};
		
			union {
				union {
					LOCK::Bits lock;
					DRDY::Bits drdy;
				};
				uint16_t bits;
			};
	};

	class TEMP_OUT_M : public DeviceRegister<0x31, 2> {
		public:
			struct DATA {
				using Bits = BitFieldReadOnly<uint16_t, 4, 12, int16_t>;
			};
		
			union {
				union {
					DATA::Bits data;
				};
				uint16_t bits;
			};
	};

}
