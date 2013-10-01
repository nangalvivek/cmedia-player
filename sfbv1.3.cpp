#include<stdio.h>
#include<pwd.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
//#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<string.h>
#include<fmod.hpp>
#include<malloc.h>
#include<fmod_errors.h>
int key_hit=1;
int no_tags=0;
char* file_ext_c=".MP3";
int no_of_dir=1,no_of_files=0,selected=1;
char* parent="..";
char current_dir[1024];
char* file_ext=".mp3";
char* last_dir;
unsigned int version;
float vol=0.8f;
unsigned int lenms,position;
bool isplaying=false;
bool ipl=false;
char path[100]="vivek";
char *message;
bool newl=false;
char file_name[255];
float per;
bool isStopped=true;
int last_playing;

char out;
//extern void w_routine(char *msg);
extern void w_routine(char *msg,char *title,int lines,int cols,int x_pos,int y_pos,int x_lno,int y_lno);\
void details();

void w_routine(char *msg,char *title="ERROR",int lines=5,int cols=40,int x_pos=(LINES-5)/2,int y_pos=(COLS-40)/2,int x_lno=2,int y_lno=3) {
	WINDOW *sub=newwin(lines,cols,x_pos,y_pos);
			wborder(sub,0,0,0,0,0,0,0,0);	
			mvwprintw(sub,0,3,title);	
			mvwprintw(sub,x_lno,y_lno,msg);
			wrefresh(sub);
			sleep(1);
			delwin(sub);
			touchwin(stdscr);
}


void ERRCHECK(FMOD_RESULT result,int l=1)
{
    if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE)
    {
//        mvprintw(10,10,"FMOD error! (%d)%s at %d\n", result, FMOD_ErrorString(result),l);
	char s[100];
	sprintf(s,"FMOD error! (%d) %s at %d\n",result, FMOD_ErrorString(result),l);
	w_routine(s,"FMOD ERROR");
	sleep(1);

    }
}

void nayi_window() {
	WINDOW *sub=newwin(5,60,(LINES-5)/2,(COLS-60)/2);
	wborder(sub,0,0,0,0,0,0,0,0);	
	mvwprintw(sub,0,3,"PATH");	
	mvwprintw(sub,1,1,"Enter the path of the folder :");
	echo();nocbreak();noraw();
	wrefresh(sub);
	wtimeout(sub,-12);
	curs_set(1);
	wmove(sub,2,2);
	wscanw(sub,"%s",path);
	curs_set(0);
	noecho();
	cbreak();raw();
//	sleep(1);
	delwin(sub);
	touchwin(stdscr);
}



//---------INITIALISE FMOD--------------
	FMOD::System     *system;
	FMOD::Sound      *sound;
	FMOD::Channel    *channel = 0;
	FMOD_RESULT       result;
//--------------------------------------

void paused(){
	if(!isStopped){
			bool paused;
                    channel->getPaused(&paused);
                    channel->setPaused(!paused);
			isStopped=false;
			isplaying=paused;
	}
}

struct dir_content {
	char name[255];
	int type;
	struct dir_content *ptr;
}*start=NULL;

void gc() {

		struct dir_content* p=start;
		struct dir_content* prev=NULL;
		while(p!=NULL) {
			prev=p;
			p=p->ptr;
			if(!prev)free(prev);
		}
		free(start);
		start=NULL;



}

void insert(char *a_name,int type){
	struct dir_content *pointer=(struct dir_content *)malloc(sizeof (struct dir_content));
	struct dir_content *q;
	pointer->ptr=NULL;
	strcpy(pointer->name,a_name);
	pointer->type=type;
	if(start==NULL)
		start=pointer;
	else {
		q=start;
		while(q->ptr!=NULL)
			q=q->ptr;
		q->ptr=pointer;
	}
	
}

void getdir(char *d_name) {
	
	if(start!=NULL)	return;
	no_of_dir=1,no_of_files=0;
	
	insert(parent,1);
	long int td;
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp=opendir(d_name))==NULL)
		return;
	chdir(d_name);

	td=telldir(dp);
	while((entry=readdir(dp))!=NULL) {
		
		lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)) {
			if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0||entry->d_name[0]=='.' ) continue;
			else {
				insert(entry->d_name,1);
				no_of_dir++;
			}
		}
		else continue;
	}

	seekdir(dp,td);
	
	while((entry=readdir(dp))!=NULL) {
		
		lstat(entry->d_name,&statbuf);
		if(!S_ISDIR(statbuf.st_mode)) {
			if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0||entry->d_name[0]=='.' ) continue;
			else {	
				insert(entry->d_name,0);
				no_of_files++;
			}
		}
		else continue;
	}
	last_dir=getcwd(current_dir,sizeof(current_dir));
	closedir(dp);
	
}

int check(char* a_name) {
	DIR *dp;
	if((dp=opendir(a_name))==NULL)
		return 0;
	else {
		closedir(dp);
	return 1;
	}
}

//-------------------------------DISPLAY FUNCTIONS--------------------------

void on_screen(int a_selected){
	
	struct dir_content *p=start;
	int count=1;
	move(1,0);
	while(p!=NULL) {int x=0,y=0;
		if(a_selected==count)attron(A_REVERSE|A_BOLD);	
		if(p->type==1){
			printw(" + %00.75s",p->name);
			getyx(stdscr,y,x);
			while(x++<COLS-1)addch(' ');
			printw("\n");
//				for(int i=0;i<10000000;i++);
			
		}
		else if(p->type==0) {
			printw("   %00.75s",p->name);
			getyx(stdscr,y,x);
			while(x++<COLS-1)addch(' ');
			printw("\n");
				//for(int i=0;i<10000000;i++);
		}
		attroff(A_REVERSE|A_BOLD);
		count++;
	p=p->ptr;
	}
	if(newl){newl=false;
		for(int i=0;i<LINES-(no_of_files+no_of_dir);i++)
			printw("\n");
	}
	
	
}

void stop() {
			result=sound->release();
			ERRCHECK(result,230);
	
			result=system->close();
			ERRCHECK(result,233);		
	
			result = system->release();
			ERRCHECK(result,236);
			
			channel=0;
			isStopped=true;
			isplaying=false;

}

void on_screen1(int offset,int a_selected) {
	clear();
	struct dir_content *p=start;
	int count=1,count_off=0;
	move(1,0);
//	count=LINES+offset-2;
	while(p!=NULL) {int x=0,y=0;
		while(count_off<=offset){
			p=p->ptr;
			count_off++;
			count++;
			continue;
		}
		if(p->type==1){
			if(a_selected==count)attron(A_REVERSE|A_BOLD);
			printw(" + %00.75s",p->name);
			getyx(stdscr,y,x);
			while(x++<COLS-1)addch(' ');
			printw("\n");
			attroff(A_REVERSE|A_BOLD);
		}
		else if(p->type==0) {
			if(a_selected==count)attron(A_REVERSE|A_BOLD);
			printw("   %00.75s",p->name);
			getyx(stdscr,y,x);
			while(x++<COLS-1)addch(' ');
			printw("\n");
			attroff(A_REVERSE|A_BOLD);
		
		}
		count++;
	p=p->ptr;
	}
	


}

void volume(int key,float inc=0.100f) {
	switch(key) {
		case '=':
			if(channel) {			
				if(vol+inc<=1.001f)
					vol+=inc;
				channel->setVolume(vol);	
			}
			break;
		case '-':
			if(channel) {	
				if(vol-inc>=-0.001f)
					vol-=inc;
				channel->setVolume(vol);
			}
			break;	
		
	}

}

//-----------------------------------------------------------------------------------

void cdir(unsigned int a_selected,char *a_path="NULL") {
	if(path!="NULL")getdir(a_path);
	long int td,td_dir;
	int count=1,count_file=0;
//--------HOUSEKEEPING--FREEING UNNECESSARY MEMORY-------
	gc();
//------------------------------------------------------
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if((dp=opendir(last_dir))==NULL)
		return;
	chdir(last_dir);
	if(a_selected==1){
		selected=1;
		getdir("..");
		clear();
		return;
	}
	td=telldir(dp);
	while((entry=readdir(dp))!=NULL) {
		
		lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)) {
			if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0||entry->d_name[0]=='.' ) continue;
			else {
				count++;
				if(count==a_selected) break;
				//mvprintw(0,0,"%d",count);
//				sleep(1);refresh();
			}
		}
		else continue;
	}
	td_dir=telldir(dp);
//	mvprintw(0,0,"%d %d",count,a_selected);sleep(2);refresh();
	seekdir(dp,td);
	
	if(a_selected>count) {
		while((entry=readdir(dp))!=NULL) {
		
		lstat(entry->d_name,&statbuf);
		if(!S_ISDIR(statbuf.st_mode)) {
			if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0||entry->d_name[0]=='.' ) continue;
			else {	
				count_file++;
				if(count_file==(a_selected-count))break;
			}
		}
		else continue;
		}
				
//	mvprintw(10,10,"%s",entry->d_name);
//	refresh();sleep(1);
		if(strstr(entry->d_name,file_ext)==NULL&&strstr(entry->d_name,file_ext_c)==NULL){
			message="Neither a mp3 nor a directory...";
			w_routine(message);
			message=NULL;
		}

		else {
		if(!isplaying) {
			result = FMOD::System_Create(&system);
			ERRCHECK(result);
			result = system->getVersion(&version);
			ERRCHECK(result);



    			result = system->init(1, FMOD_INIT_NORMAL, 0);
    			ERRCHECK(result);
			
//			result=system->
    			result = system->createSound(entry->d_name, FMOD_ACCURATETIME|FMOD_MPEGSEARCH|FMOD_DEFAULT|FMOD_CREATESTREAM, 0, &sound);
    			ERRCHECK(result);
			
			strcpy(file_name,entry->d_name);
			
//			sound->setMusicSpeed(90.0f);
			result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
			ERRCHECK(result);
			
			result=sound->getLength(&lenms,FMOD_TIMEUNIT_MS);
			ERRCHECK(result);

			
			result = channel->setVolume(vol);		
			ERRCHECK(result);
			isStopped=false;
			isplaying=true;	
			last_playing=selected;
			details();
			
		}
		else {
			stop();
			cdir(selected);	
		}




		}


		on_screen(selected);
		refresh();
		return;
	}
	selected=1;
	//	find_values(entry->d_name);
	seekdir(dp,td_dir);
//	mvprintw(10,10,"%s",entry->d_name);
//	refresh();sleep(1);
	clear();
	getdir(entry->d_name);return;


	
	
}

void wdraw_box(WINDOW* win,int y1,int x1,int y2,int x2) {
	mvwaddch(win,y1,x1,ACS_ULCORNER);
	mvwaddch(win,y1,x2,ACS_URCORNER);
	mvwaddch(win,y2,x1,ACS_LLCORNER);
	mvwaddch(win,y2,x2,ACS_LRCORNER);
	mvwhline(win,y1,x1+1,ACS_HLINE,x2-x1-1);
	mvwhline(win,y2,x1+1,ACS_HLINE,x2-x1-1);
	mvwvline(win,y1+1,x1,ACS_VLINE,y2-y1-1);
	mvwvline(win,y1+1,x2,ACS_VLINE,y2-y1-1);


}

//#define PER (float)position/(float)lenms

void seek(int k) {
	switch(k){
		case KEY_RIGHT:
			if(channel) {
				if(position+10000<=lenms)
					position+=10000;
				channel->setPosition(position,FMOD_TIMEUNIT_MS);	
			}
			break;
		case KEY_LEFT:
			if(channel) {
				if(position-10000>=0)
					position-=10000;
				channel->setPosition(position,FMOD_TIMEUNIT_MS);
			}
			break;
	}
}

void progress_border(WINDOW* win,int y1,int x1,int y2,int x2,float a_per,int write) {

	wdraw_box(win,y1+1,x1-1,y2+1,x2);
	
	int start=x1;
	int a;
	int limit=(per/100)*(x2-x1);
//	char ab[10];
	//sprintf(ab,"%d %d",start,limit);
//	w_routine(ab);
	if(!isStopped)
	wattron(win,A_REVERSE);
//	curs_set(1);
	wmove(win,y1+2,start+1);
	for(a=start;a<start+limit;a++){
		waddch(win,' ');
	}
	wattroff(win,A_REVERSE);

	for(int x=a;x<start+(x2-x1-1);x++)waddch(win,' ');
//	touchwin(win),wrefresh(win);sleep(5);



}
void details() {
	bool have=false;
	WINDOW *sub=newwin(LINES,COLS,0,0);
	keypad(sub,1);
	wmove(sub,2,0);
	int count=0;
	FMOD_TAG tag;
	result=sound->getNumTags(&no_tags,0);
	ERRCHECK(result);
	int width;
	int height;
	int y1;
	int x1;
	int y2;
	int x2;
	int endy;
	int endx;


	int key=0;

while(1) {
	width=COLS-(COLS/10)+2;
	height=(LINES/3)+3;
	y1=LINES/2-height;
	y2=height;
	x1=(COLS/2)-(width/2);
	x2=(COLS/2)+(width/2);
	endy=y2-1;
	endx=x2;
	wdraw_box(sub,y1,x1,y2-1,x2);
	system->update();
	if(channel) {
		result=channel->getPosition(&position,FMOD_TIMEUNIT_MS);	
		ERRCHECK(result,__LINE__);

	
	}
	per=(float)position/(float)lenms;
	per=per*100;
	int line_no=y1+2;
	mvwprintw(sub,line_no++,x1+3,"%-10s  :  %s","File Name",file_name);
	mvwprintw(sub,line_no++,x1+3,"%-10s  :  %s","State",(isStopped?"Stopped":isplaying?"Playing":"Paused "));
	mvwprintw(sub,line_no++,x1+3,"%-10s  :  %02d:%02d:%02d/%02d:%02d:%02d (%02d.%02d %)","Position",position/1000/60,position/1000%60,position/10%100,lenms/1000/60,lenms/1000%60,lenms/10%100,(int) per,(int)((per-(int)per)*100));

	mvwprintw(sub,line_no++,x1+3,"%-10s  :  %04.2f %%    ","Volume",vol<0.000f?vol*(-100):vol*100);

//	
	progress_border(sub,endy-4,x1+3,endy-2,endx-3,per,endy-3);
	int startx=x1;
	 endx=x2;
	int starty=endy+2;
	endy=starty+(0.45f)*((float)(LINES-1));

	line_no=starty+2;
	


//	for(int i=0;i<3;i++){
	if(!have){
		sound->getTag("TITLE",0,&tag);
		if (tag.datatype == FMOD_TAGDATATYPE_STRING)
	        mvwprintw(sub,line_no++,startx+3,"%-10s : %s\n", "Title", tag.data),count++;
	//	*tags++;

		sound->getTag("ARTIST",0,&tag);
		if (tag.datatype == FMOD_TAGDATATYPE_STRING)
                mvwprintw(sub,line_no++,startx+3,"%-10s : %s\n", "Artist", tag.data),count++;

		sound->getTag("ALBUM",0,&tag);
		if (tag.datatype == FMOD_TAGDATATYPE_STRING)
                mvwprintw(sub,line_no++,startx+3,"%-10s : %s\n", "Album", tag.data),count++;

		sound->getTag("TYER",0,&tag);
		if (tag.datatype == FMOD_TAGDATATYPE_STRING)
                mvwprintw(sub,line_no++,startx+3,"%-10s : %s\n", "Year", tag.data),count++;

		sound->getTag("TCOM",0,&tag);
		if (tag.datatype == FMOD_TAGDATATYPE_STRING)
                mvwprintw(sub,line_no++,startx+3,"%-10s : %.50s\n", "Comments", tag.data),count++;

		if(!count) mvwprintw(sub,line_no,startx+3,"\tSorry...no track information found...\n" );
		have=true;
	}



//	}	
	wdraw_box(sub,starty,startx,endy,endx);
	wborder(sub,0,0,0,0,0,0,0,0);
	mvwprintw(sub,0,3," NOW PLAYING ");
	mvwprintw(sub,starty,startx+3," TRACK INFORMATION ");
	wtimeout(sub,100);


	key=wgetch(sub);
	if(key==KEY_LEFT||key==KEY_RIGHT)seek(key);
	else if(key==KEY_UP||key==KEY_DOWN)volume(key==KEY_UP?'=':'-',0.100f);
	else if(key=='='||key=='-')volume(key,0.005f);
	else if(key==32)paused();
	else if(key=='q'){key_hit='q';break;}
	else if(key=='s')stop();	
	else if(key=='p')cdir(last_playing);
	else if(key=='d')break;


}

	curs_set(0);
	
	cbreak();raw();
//	sleep(1);
	delwin(sub);
	touchwin(stdscr);


}

	

void progress(unsigned int a_lenms,unsigned int a_pos) {

	per=(float)a_pos/(float)a_lenms;
	attron(A_REVERSE);
	move(LINES-1,0);
	int limit=(COLS-0)*per;
	for(int x=0;x<limit;x++){
		addch(' ');
	}
	attroff(A_REVERSE);
	per=per*100;

	char string[25];
	sprintf(string,"%02d:%02d:%02d/%02d:%02d:%02d (%02d.%02d%%)",position/1000/60,position/1000%60,position/10%100,lenms/1000/60,lenms/1000%60,lenms/10%100,(int) per,(int)((per-(int)per)*100));
//	w_routine(string);
	int l=strlen(string);
	int start_x=(COLS-30)/2;
	move(LINES-1,start_x);
	attron(A_BOLD);
	for(int x=start_x,i=0;x<start_x+limit,i<l;x++,i++){
		if(start_x+i<limit)attron(A_REVERSE);
		addch(string[i]);
		attroff(A_REVERSE);
	}
	attroff(A_BOLD);

}

void input(int key_hit) {
		
	system->update();
	if(channel) {
		result=channel->getPosition(&position,FMOD_TIMEUNIT_MS);	
		ERRCHECK(result,__LINE__);

	
	}
	if(channel==0)isStopped=true;
	else isStopped=false;

	
	switch(key_hit) {
		case KEY_DOWN:
			mvprintw(LINES-1,COLS-5,"DN");			refresh();//sleep(1);	
			getdir(current_dir);
			if(selected==(no_of_files+no_of_dir)) selected=1;
			else selected++;
			if(selected>LINES-2) {
//				mvprintw(10,20,"%d %d",selected-LINES+2,selected);refresh();sleep(1);
				on_screen1(selected-LINES+2,selected);
			}
			else
				on_screen(selected);


			break;

		case KEY_UP:
			mvprintw(LINES-1,COLS-5,"UP");	refresh();
			getdir(current_dir);
			if(selected==1) selected=(no_of_files+no_of_dir);
			else selected--;
			if(selected>LINES-2) {
//				mvprintw(10,20,"%d %d",selected-LINES+2,selected);refresh();sleep(1);
				on_screen1(selected-LINES+2,selected);
			}
			else
				on_screen(selected);

			break;

		case 'q':
			
			endwin();
			break;
	
		case 10:
			cdir(selected);
			on_screen(1);
			break;
		case 32:
			if(channel) 
			paused();
			break;
		case '=':
		case '-':
			volume(key_hit);

		case KEY_LEFT:
		case KEY_RIGHT:
			seek(key_hit);
			break;	
		case KEY_BACKSPACE:
			cdir(1);
		case KEY_HOME:
			on_screen((selected=1));
			break;
		case KEY_END:
			selected=(no_of_files+no_of_dir);
			on_screen1(selected-LINES+2,selected);
			break;
		case '/':
			nayi_window();
			if(check(path)==1){
					gc();
			}
			else {w_routine("WRONG PATH"); break;}
			getdir(path);
			selected=1;
			newl=true;
			on_screen(1);
			
			break;
		case 'd':
		case 'i':
			int k;
			if(channel)
				details();
			break;
		case 's':
			if(channel)
			stop();
			break;

	}
	wborder(stdscr,0,0,0,0,0,0,0,0);
	mvprintw(0,3," FILE BROWSER - %0.50s.... %d %d",current_dir,(no_of_files+no_of_dir),LINES);
	if(channel){
		channel->isPlaying(&ipl);
		if(ipl)
		progress(lenms,position);
	}
	refresh();
}


	uid_t uid;


int main() {

	struct passwd* pw;
	uid=getuid();
	pw=getpwuid(uid);
	char *home=pw->pw_dir;
	strcat(home,"/Desktop");
	initscr();
	noecho();raw();
	keypad(stdscr,1);

//	int i;
//	printw("hi");
	curs_set(0);
	getdir(home);
	on_screen(1);
	wborder(stdscr,0,0,0,0,0,0,0,0);
	mvprintw(0,3," FILE BROWSER - %0.50...s ",current_dir);
	
	while(1) {

		if(key_hit=='q')break;
		timeout(100);
		key_hit=getch();
		input(key_hit);
	}
	wborder(stdscr,0,0,0,0,0,0,0,0);

	endwin();
	return 0;
}
