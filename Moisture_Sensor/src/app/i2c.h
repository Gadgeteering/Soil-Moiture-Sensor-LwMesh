
void i2c_init(void);

#define SLAVE_ADDRESS 0x40
#define DATA_LENGTH 3
#define TIMEOUT 1000

static uint8_t write_buffer[DATA_LENGTH] = {
	0xE5, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
};
static uint8_t read_buffer[DATA_LENGTH];



/* Init i2c packet. */
struct i2c_master_packet packet = {
	.address = SLAVE_ADDRESS,
	.data_length = DATA_LENGTH,
	.data = write_buffer,
	.ten_bit_address = false,
	.high_speed = false,
	.hs_master_code = 0x0,
};