#include "includes.h"

void Mode0_DebugCamera(void);
void Mode1_SendVideo(void);
void Mode2_GO(void);
void Mode3_Andriod(void);

void main(void)
{
	init_all_and_POST();
	if(mode==0)
		Mode0_DebugCamera();//图像显示屏显示，车速20，显示offset RoadType，舵机打角，wifi_car_action不激活
	else if(mode==1)
		Mode1_SendVideo();//推车录图像，仅摄像头图像发上位机
	else if(mode==2)
		Mode2_GO();//速度20，WIFI读卡循迹超声全开，图像不显示不发送
	else if(mode==3)
		Mode3_Andriod();//远程模式，上位机遥控车
}
void Mode0_DebugCamera(void)
{
	set_speed_target(20);
	
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	
	LCD_write_english_string(96,0,"T");
	LCD_write_english_string(96,2,"R");
	
	for (;;)
	{
		if(fieldover)
		{
			fieldover=0;
			
			FindBlackLine();
			Display_Video();



			if(target_offset<0)
				LCD_write_english_string(96,1,"-");
			else LCD_write_english_string(96,1,"+");
			LCD_Write_Num(105,1,ABS(target_offset),2);
			LCD_Write_Num(105,2,RoadType,2);
			SteerControl();

			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
	}
}

void Mode1_SendVideo(void)
{
	SetupCCD();	
	g_f_enable_speed_control = 0;
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断

	for (;;)
	{

		if(fieldover)
		{
			fieldover=0;
			FindBlackLine();
			Send_CCD_Video();
			
			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
		
	}

}

void Mode2_GO(void)
{
	set_speed_target(20);
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	
	for (;;)
	{

		/* 执行远程命令 */
		if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
		{
			g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
			
			execute_remote_cmd(remote_frame_data+5);
		}

		/* 整车动作控制 */
		control_car_action();
		fieldover=1;//尝试
		if(fieldover)
		{
			fieldover=0;
			
			FindBlackLine();
			if(g_f_red==1&&g_f_stopline==1)
			{
				set_speed_target(0);
				D6=~D6;
			}
			else
				set_speed_target(20);
			
			SteerControl();
			
			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
	}
}
void Mode3_Andriod(void)
{
	for (;;)
	{
		/* 执行远程命令 */
		if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
		{
			g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
			
			execute_remote_cmd(remote_frame_data+5);
		}

		/* 整车动作控制 */
//		control_car_action();

		/* 报告在线 */
//		report_online();


	}

}
