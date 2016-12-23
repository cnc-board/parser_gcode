
#include "../headers/Zynqcomm.hpp"

Zynq_comm::Zynq_comm(bool simulate) : Simulation_mode(simulate)
{
      if(Simulation_mode==false)
      {
        comm_FD = open("/dev/mem", O_RDWR | O_SYNC);
        if(comm_FD <0) {
                perror("cannot open file");
                exit(-1);
        }
        generator_addr = (uint32_t*)(mmap( 0, 4096, PROT_WRITE | PROT_READ, MAP_SHARED,comm_FD,0x43C00000)); //axi
        if (check_comm()) {
                perror("cannot comunicate with zynq");
                exit(-1);
        }
      }
        else
        {

        }
}

Zynq_comm::~Zynq_comm() {
        if(Simulation_mode==false)
        {
        cout << "Unmaping Zynq ... " << flush;
        if(munmap(generator_addr,1) == -1) {
                perror("cannot munmap");
                exit(-1);
        }
        cout << "Done" << endl;
        cout << "Closing Zynq ... " << flush;
        close(comm_FD);
        cout << "Done" << endl;
        comm_FD=-1;
      }
      else
      {
        cout << "Unmaping Zynq ... " << flush;
        cout << "Done" << endl;
        cout << "Closing Zynq ... " << flush;
        cout << "Done" << endl;
      }
        //exit(-1);
}

void Zynq_comm::transmit_vector(Accel_vector& vect)
{
  if(Simulation_mode==false)
  {
        generator_addr[R_3]=(vect.Accel_y&0xFFFF)<<16 | (vect.Accel_x&0xFFFF);
        generator_addr[R_2]=(vect.Accel_a&0xFFFF)<<16 | (vect.Accel_z&0xFFFF);
        generator_addr[R_1]=vect.epoch_stop&0xFFFFFFFF;
        generator_addr[R_0]=(1<<bit_write_fifo)|((vect.epoch_stop>>32)&0xFFFF);
      }
}
void Zynq_comm::execute_fifo_list()
{
  if(Simulation_mode==false)
  {
        generator_addr[R_control]=(1<<bit_startcycle);
      }
}

void Zynq_comm::execute_reset_off()
{

}

void Zynq_comm::send_reset_FPGA()
{

}

unsigned int Zynq_comm::get_fifo_fill()
{
  if(Simulation_mode==false)
  {
        return generator_addr[R_3]*100/1024;
  }
  else
  {
        return 50;
  }
}

bool Zynq_comm::get_fifo_empty()
{
  if(Simulation_mode==false)
  {
        return generator_addr[R_3]==0;
  }
  else
  {
        return true;
  }
}

Position_Vector Zynq_comm::get_axe_position()
{
  int32_t x,y,z,a;
  if(Simulation_mode==false)
  {
      generator_addr[R_position]=0;
      x=generator_addr[R_position];
      generator_addr[R_position]=1;
      y=generator_addr[R_position];
      generator_addr[R_position]=2;
      z=generator_addr[R_position];
      generator_addr[R_position]=3;
      a=generator_addr[R_position];
      return Position_Vector(x,y,z,a);
  }
  else
  {
      return Position_Vector(posxdeb,0,0,0);
  }
}

int Zynq_comm::check_comm()
{
        return 0;
}
