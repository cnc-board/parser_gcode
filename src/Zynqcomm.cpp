
#include "../headers/Zynqcomm.hpp"
static int comm_FD=-1;
uint32_t* generator_addr = NULL;
int open_driver_comm()
{
        comm_FD = open("/dev/mem", O_RDWR | O_SYNC);
        if(comm_FD <0) {
                perror("cannot open file");
                return -1;
        }
        generator_addr = (uint32_t*)(mmap( 0, 4096, PROT_WRITE | PROT_READ, MAP_SHARED,comm_FD,0x43C00000)); //axi
        if (check_comm()) {
                perror("cannot open file");
                return -1;
        }
        return 0;
}

void transmit_vector(Accel_vector& vect)
{
        generator_addr[R_3]=(vect.Accel_y&0xFFFF)<<16 | (vect.Accel_x&0xFFFF);
        generator_addr[R_2]=(vect.Accel_a&0xFFFF)<<16 | (vect.Accel_z&0xFFFF);
        generator_addr[R_1]=vect.epoch_stop&0xFFFFFFFF;
        generator_addr[R_0]=(1<<bit_write_fifo)|((vect.epoch_stop>>32)&0xFFFF);
}
void execute_fifo_list()
{
        generator_addr[R_control]=(1<<bit_startcycle);
}

void execute_reset_off()
{

}

void send_reset_FPGA()
{

}

unsigned int get_fifo_fill()
{
 return generator_addr[R_3]*100/1024;
}
int check_comm()
{
        return 0;
}

int close_driver_comm()
{
        if(munmap(generator_addr,1) == -1) {
                perror("cannot munmap");
                return -1;
        }
        close(comm_FD);
        comm_FD=-1;
        return 0;
}
