#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

unsigned char mode;
unsigned char bits = 8;
unsigned int speed = 5000;

static void send(int fd, short val)
{
	int status;
    char highByte = (val >> 6) | 0b00110000;
	char lowByte = val << 2 & 0b11111100;
    
	unsigned char tx[] = {
        highByte, 
        lowByte
	};
	
	unsigned char rx[2] = {0, 0};
	
	struct spi_ioc_transfer spi_message = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 2,
		.delay_usecs = 50,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	status = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_message);
	
	if (status < 1)
	{
		perror("Failed to send message.");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	int status = 0;
	int fd;

	fd = open("/dev/spidev0.0", O_RDWR);
	
	if (fd < 0)
	{
		perror("Failed to open device");
		exit(1);
	}

	status = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	
	if (status == -1)
	{
		perror("Failed to set SPI mode");
		exit(1);
	}

	status = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	
	if (status == -1)
	{
		perror("Failed to set bits per word.");
		exit(1);
	}

	status = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	
	if (status == -1)
	{
		perror("can't set max speed hz");
		exit(1);
	}

	send(fd, 1023);
	close(fd);
	return status;
}

