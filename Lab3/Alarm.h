/*****************Alarm.h****************
*This file contains function definitions
*for controlling the alarm
*Paul Cozzi and Eric Li, 9/14/2016
*****************************************/
/*****************alarms*****************
*array used to hold the times for the alarms set
*****************************************/
static int32_t alarms[6];

/*****************isSet*****************
*array to tell if each alarm is set
*****************************************/
static int32_t isSet[6];

/**********outputTime********************
*This function is used to print a time to the ST7735
*it is assumed that all formatting (font size, cursor position, etc)
*was previously done
*****************************************/
static void outputTime(int32_t time);			//Paul

/**********Alarm_Trigger***********************
*a function to turn on sound on the speaker
*index is the index in the alarms array to trigger
*****************************************/
static void Alarm_Trigger(int32_t index); 	//Eric

/**********Alarm_unTrigger***********************
*a function to turn off sound on the speaker
*****************************************/
void Alarm_unTrigger(void);								//Eric

/**********AlarmInit***********************
*a function to initialize everything the alarm module will need
*****************************************/
void Alarm_Init(void);										//Eric

/***********alarmSet********************
*Provides an interface for the user to set the
*time of an alarm
*****************************************/
static void alarmSet(int32_t index);			//Paul

/***********Alarm_Menu********************
*Displays a menu where alarms can be turned on/off
*or set/unset
*****************************************/
void Alarm_Menu(void);											//Paul

/******************Alarm_SoundIfActive***
*Sounds an alarm if the given time is set as an alarm
*****************************************/
void Alarm_SoundIfActive(int32_t time);			//Eric



