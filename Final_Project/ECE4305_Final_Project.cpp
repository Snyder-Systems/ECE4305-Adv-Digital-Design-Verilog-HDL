/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include <cstdlib>


int num_array[10] = {48,49,50,51,52,53,54,55,56,57};
int score_osd[12] = {76,97,115,116,0,83,99,111,114,101,58, 0};
int title [13] = {70, 108, 97,112,112,105,110,103, 0 ,66,105,114,100};
int play_title[21] = {80, 114, 101, 115, 115, 0, 34, 115,112,97,99,101,34, 0, 116,111, 0, 80, 108, 97, 121};
int paused[6] = {80, 97,117, 115, 101, 100};


void pipe_check(SpriteCore *pipe_p){
	int x, y;

	   // slowly move mouse pointer
	   pipe_p->bypass(0);
	   pipe_p->wr_ctrl(0x00);  //cutout1
	   x = 450;
	   y = 0;
	   	  pipe_p->move_xy(x, y);
	   	  sleep_ms(5000);
	   pipe_p->wr_ctrl(0x01); //cutout2
	   x = 300;
	      pipe_p->move_xy(x, y);
	      sleep_ms(5000);
	   x = 100;
	   pipe_p->wr_ctrl(0x02); //cutout3
	      pipe_p->move_xy(x, y);
	      sleep_ms(5000);
	   x = 100;
	      pipe_p->wr_ctrl(0x03);
	      pipe_p->move_xy(x, y);
	      sleep_ms(5000);
	      pipe_p->bypass(1);
	      sleep_ms(2000);
}

void pipe_move_check(SpriteCore *pipe_p){
	int x, y;

	   // slowly move mouse pointer
	   pipe_p->bypass(0);
	   pipe_p->wr_ctrl(0x01);  //animation; blue ghost
	   x = 640;
	   y = 0;
	   for (int i = 0; i < 70; i++) {
	      pipe_p->move_xy(x, y);
	      sleep_ms(100);
	      x = x - 10;
	   }

}

int birdx_control(XadcCore* adc_p, int birdx, int acceleration){
	int x_value = 10 * adc_p->read_adc_in(0);
	int new_birdx;

	if (x_value > 7)
		new_birdx = birdx - 2 -acceleration;
	else if(x_value < 3)
		new_birdx = birdx + 2 + acceleration;
	else
		new_birdx = birdx;
	//boundary check
	if (new_birdx < 0){
		new_birdx = 0;
	}
	else if (new_birdx > 640){
		new_birdx = 640;
	}
	return new_birdx;
}

int birdy_control(XadcCore* adc_p, int birdy, int acceleration){
	int y_value = 10 * adc_p->read_adc_in(1);
	int new_birdy;

	if (y_value > 7)
		new_birdy = birdy + 3 + acceleration;
	else if(y_value < 3)
		new_birdy = birdy - 3 - acceleration;
	else
		new_birdy = birdy;
	//boundary check
	if (new_birdy < 0){
		new_birdy = 0;
	}
	else if (new_birdy > 459){
		new_birdy = 459;
	}
	return new_birdy;
}

int pipe_generator(SpriteCore* pipe_p){
	int pipe_cutout = rand() % 4;
	uart.disp(pipe_cutout);
	uart.disp("\n\r");
	int pipe_start = 640;
	pipe_p->bypass(0);
	pipe_p->wr_ctrl((int32_t)pipe_cutout);
	pipe_p->move_xy(pipe_start, 0);
	return pipe_cutout;
}

int move_pipe(SpriteCore* pipe_p, int pipex, int acceleration){
	int new_pipex;

	new_pipex = pipex - 5 - acceleration;

	return new_pipex;

}
int pipe_check(SpriteCore* pipe_p, int pipex){
	if (pipex <= 0){
		pipe_p->bypass(1);
		return 0;
	}
	else
		return 1;
}

int collision_detect (int pipe_cutout, int pipex, int birdx, int birdy){
	int pipe_ceiling, eye_hit, body_hit;
	eye_hit = 0;
	body_hit = 0;
	if (((birdx+2) >= (pipex+2) && (pipex+60) >=(birdx + 2)) || ((birdx + 30) >= (pipex+2) && (pipex + 60) >=(birdx + 30)) ){
		if (pipe_cutout == 0){
			pipe_ceiling = 11;
			//eye hitbox
			if (((birdx + 15) >= (pipex+2) && (pipex+60) >=(birdx + 15)) || ((birdx + 26) >= (pipex+2) && (pipex + 60) >=(birdx + 26)) ){
					if(birdy <= pipe_ceiling)
						eye_hit = 1;
			}
			//body hitbox
			if (((birdx + 6) >= (pipex) && (pipex+60) >= (birdx + 6)) || ((birdx + 29) <= (pipex+60) && (pipex) <= (birdx + 29)) ){
					if((birdy+6) <= pipe_ceiling || (birdy + 16) >= pipe_ceiling +99)
						body_hit = 1;
			}
			if(eye_hit == 1 || body_hit == 1)
				return 1;
		}

		else if (pipe_cutout == 1){
			pipe_ceiling = 131;
			//eye hitbox
			if (((birdx + 15) >= (pipex+2) && (pipex+60) >=(birdx + 15)) || ((birdx + 26) >= (pipex+2) && (pipex + 60) >=(birdx + 26)) ){
					if(birdy <= pipe_ceiling)
						eye_hit = 1;
			}				//body hitbox
			if (((birdx + 6) >= (pipex) && (pipex+60) >= (birdx + 6)) || ((birdx + 29) <= (pipex+60) && (pipex) <= (birdx + 29)) ){
					if((birdy+6) <= pipe_ceiling || (birdy + 16) >= pipe_ceiling +99)
						body_hit = 1;
			}
			if(eye_hit == 1 || body_hit == 1)
					return 1;
		}


		else if (pipe_cutout == 2){
			pipe_ceiling = 251;
			//eye hitbox
			if (((birdx + 15) >= (pipex+2) && (pipex+60) >=(birdx + 15)) || ((birdx + 26) >= (pipex+2) && (pipex + 60) >=(birdx + 26)) ){
					if(birdy <= pipe_ceiling)
						eye_hit = 1;
			}
			//body hitbox
			if (((birdx + 6) >= (pipex) && (pipex+60) >= (birdx + 6)) || ((birdx + 29) <= (pipex+60) && (pipex) <= (birdx + 29)) ){
					if((birdy+6) <= pipe_ceiling || (birdy + 16) >= pipe_ceiling +99)
						body_hit = 1;
			}
			if(eye_hit == 1 || body_hit == 1)
					return 1;
		}
		else{
			pipe_ceiling = 371;
			//eye hitbox
			if (((birdx + 15) >= (pipex+2) && (pipex+60) >=(birdx + 15)) || ((birdx + 26) >= (pipex+2) && (pipex + 60) >=(birdx + 26)) ){
					if(birdy <= pipe_ceiling)
						eye_hit = 1;
			}
			//body hitbox
			if (((birdx + 6) >= (pipex) && (pipex+60) >= (birdx + 6)) || ((birdx + 29) <= (pipex+60) && (pipex) <= (birdx + 29)) ){
					if((birdy+6) <= pipe_ceiling || (birdy + 16) >= pipe_ceiling +99)
						body_hit = 1;
			}
			if(eye_hit == 1 || body_hit == 1)
					return 1;

		}
	}

	return 0;
}

void score_update (OsdCore* osd_p, int score){
	int ones, tens, hundreds;
	ones = score % 10;
	tens =  (score/10) % 10;
	hundreds = (score/100) % 10;

	osd_p->wr_char(76, 0, num_array[hundreds]);
	osd_p->wr_char(77, 0, num_array[tens]);
	osd_p->wr_char(78, 0, num_array[ones]);
}
//starts the bird at the left of the screen at the beginning of the game
int start_pos_bird(SpriteCore* bird_p, int birdx, int birdy){

	for (int i = 0; i < 240; i++){
		bird_p->move_xy(birdx - i, birdy);
		sleep_ms(5);
	}

	return birdx - 239;
}
//sets color of bird using PS2 protocol (keyboard)
void bird_color(SpriteCore* bird_p, char keychar){
	if (keychar == '4')
		bird_p->wr_ctrl(0x04);
	else if (keychar == '1')
		bird_p->wr_ctrl(0x0c);
	else if (keychar == '2')
		bird_p->wr_ctrl(0x14);
	else if (keychar == '3')
		bird_p->wr_ctrl(0x1c);
}



// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore pipe(get_sprite_addr(BRIDGE_BASE, V5_USER5), 1024);
SpriteCore bird(get_sprite_addr(BRIDGE_BASE, V4_USER4), 4096);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));

int main() {
	int birdx, birdy, pipex, pipe_flag, pipe_cutout, collision_flag, score,score_flag, acceleration, pause;
	char keychar = '0';
	while(1){
	pause = 0;
	acceleration = 0;
	score = 0;
	score_flag = 0;
	birdx = 290;
	birdy = 240;
	pipe_flag = 0;
	pipe.bypass(1);
	osd.set_color(0xfff, 0);
	bird.move_xy(birdx,birdy);
	//title menu
	for (int i = 0; i < 21; i++){
		if (i < 13){
			osd.wr_char(32+i,10, title[i]);
		}
		osd.wr_char(28+i,20, play_title[i]);
	}
	while(1){
		frame.bypass(1);
		bar.bypass(1);
		gray.bypass(1);
		ghost.bypass(1);
		mouse.bypass(1);
		ps2.get_kb_ch(&keychar);
		if (keychar == '1' || keychar == '2' ||keychar == '3' ||keychar == '4')
			bird_color(&bird, keychar);
		if (sw.read(0) || keychar == ' '){
			for (int i = 0; i < 21; i++){
				if (i < 13){
					osd.wr_char(32+i,10, 0);
				}
				osd.wr_char(28+i,20, 0);
			}
			birdx = start_pos_bird(&bird, birdx, birdy);
			keychar = '0';
			break;
		}
	}
	//end title loop

	for(int i = 0; i < 12; i++){
		if (i < 5)
			osd.wr_char(64 + i, 0, 0);
		else
			osd.wr_char(64 + i, 0, score_osd[i]);
	}
	score_update (&osd, score);

   while (1) {
      frame.bypass(1);
      bar.bypass(1);
      gray.bypass(1);
      ghost.bypass(1);
      mouse.bypass(1);
      ps2.get_kb_ch(&keychar);
      if (keychar == 'p')
      	pause = 1;

      while (pause == 1){
    	  for (int i = 0; i < 21; i++){
    	  	if (i < 6){
    	  		osd.wr_char(36+i, 10, paused[i]);
    	  	}
    	  	osd.wr_char(28+i,20, play_title[i]);
    	  	}
      	ps2.get_kb_ch(&keychar);
      	if (keychar == ' '){
      		for (int i = 0; i < 21; i++){
      			if (i < 6){
      				osd.wr_char(36+i,10, 0);
      			}
      				osd.wr_char(28+i,20, 0);
      		}
      		pause = 0;
      	}
      }
      if (pipe_flag == 0){
    	  score_flag = 0;
    	  pipe_cutout = pipe_generator(&pipe);
    	  pipe_flag = 1;
    	  pipex = 640;
      }

      birdx = birdx_control(&adc, birdx, acceleration);
      birdy = birdy_control(&adc, birdy, acceleration);
      bird.move_xy(birdx,birdy);
      pipex = move_pipe(&pipe, pipex, acceleration);
      pipe.move_xy(pipex, 0);
      collision_flag = collision_detect(pipe_cutout,pipex, birdx, birdy);

      if(collision_flag == 1){
    	  collision_flag = 0;
    	  sleep_ms(5000); //wait 5 seconds
    	  for(int i = 0; i < 12; i++)
    	  		osd.wr_char(64 + i, 0, score_osd[i]);
    	  	score_update (&osd, score);
    	  break; //reset the game
      }

      pipe_flag = pipe_check(&pipe, pipex);
      if (((birdx >= pipex + 64) || pipe_flag == 0) && score_flag == 0){
          	  score++;
          	  score_update (&osd, score);
          	  score_flag = 1;
          	  if ((score % 5) == 0)
          		  acceleration++;
            }

      sleep_ms(20);

   }
	}
} //main
