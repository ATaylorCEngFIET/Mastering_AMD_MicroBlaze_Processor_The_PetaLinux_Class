#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdio.h>

#define ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

static const char *spi_device = "/dev/spidev0.0";

int main()
{

	static uint8_t bits = 32;
	static uint32_t speed = 2000000;
	static uint32_t mode = SPI_MODE_0 ;
	static uint16_t delay = 1;

	int spi;
	int ret = 0;
	uint32_t  spi_tx_buf[1];
	uint32_t  spi_rx_buf[1];

	struct spi_ioc_transfer tr = {
	        .tx_buf = (uint32_t) spi_tx_buf,
			.rx_buf = (uint32_t) spi_rx_buf,
	        .len = 4, //ARRAY_SIZE(spi_tx_buf),
	        .delay_usecs = delay,
	        .speed_hz = 0,
	        .bits_per_word = 0,
	    };

    //Open SPI
    spi = open(spi_device, O_RDWR);
	if (spi < 0)
		printf("can't open SPI device\n\r");

	ret = ioctl(spi, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		printf("can't set spi wr mode\n\r");

	ret = ioctl(spi, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		printf("can't get spi rd mode\n\r");

	ret = ioctl(spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("can't set bits per word\n\r");

	ret = ioctl(spi, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("can't get bits per word\n\r");

	ret = ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("can't set max speed hz\n\r");

	ret = ioctl(spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("can't get max speed hz\n\r");

	spi_tx_buf[0] = (uint32_t) 0x08000001; //enable sampling
	ret = ioctl(spi, SPI_IOC_MESSAGE(1), &tr);
	if (ret == -1)
		perror("write error set up:");

	spi_tx_buf[0] = (uint32_t) 0x04000003; //enable sampling
	ret = ioctl(spi, SPI_IOC_MESSAGE(1), &tr);
	if (ret == -1)
		perror("write error set up:");

	spi_tx_buf[0] = (uint32_t) 0x03fff000; //enable sampling

	ret = ioctl(spi, SPI_IOC_MESSAGE(1), &tr);
	if (ret == -1)
		perror("write error:");

	uint16_t output=0;
	uint32_t data_out = 0x03f00000;
	while(1){

		if (output == 0xfff){
			output = 0x0;
		} else {
			output ++;
		}

		spi_tx_buf[0] = (uint32_t) data_out | (output <<8); //enable sampling

		ret = ioctl(spi, SPI_IOC_MESSAGE(1), &tr);
		if (ret == -1)
			perror("write error:");

	}

    return 0;
}
