#include <conio.h>
#include <dos.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "keys.h"
#include "conio.h"
#include "mouse.h"

void createscreen(void);
void refreshoperation(void);
void input(int,int,int,int);
void control(void);
void clearplace(void);
int  outresult(int,int);
void clearbase(void);
void savebase(void);
void visio(void);
void createvisio(void);
void refreshvisio(void);
void errorsound(void);

char *RR[]=
{
  "█   █    ██      ███    ██     ████    ████   █  █    █",
  "██ ██   █  █    █  █   █  █    █  █    █      █  █    █",
  "█ █ █   █  █    █  █   █  █    █  █    ███    █  █    █",
  "█   █   █  █    █  █   █  █    █  █    █      █  █",
  "█   █    ██    ██  █    ██    ██████   ████   █████   █",
  "                              █    █              █"
};
char *LL[]=
{
  "█  █   ████   ███    ████   ███    █  █    ██",
  "█  █   █      █  █   █      █  █   █  █   █  █",
  "████   ███    ███    ███    ███    ████   █  █",
  "█  █   █      █  █   █      █      █  █   █  █",
  "█  █   ████   ███    ████   █      █  █    ██    █   █   █"
};
char *TT[]=
{
  "███  ███  ████ █   █  ███   ███  █    █ █ █ █   ███  ██",
  "█  █ █  █ █    ██ ██ █  █   █  █ █    █ █ █ █  █  █ █  █",
  "███  ███  ███  █ █ █  ███   ███  ███  █ █ █ █  █  █ █  █",
  "█  █ █    █    █   █   ██   █  █ █  █ █ █ █ █  █  █ █  █",
  "███  █    ████ █   █  █ █   ███  ███  █ █████ ██  █  ██"
};
char *EE[]=
{
  "███   ██  █ █ █  ██     ██  █  █  ████ █  █ █  █  ██",
  "█  █ █  █ █ █ █ █  █   █  █ █  █  █    █  █ █ █  █  █",
  "███  ████ █ █ █ ████   █  █ █  █  ███  ████ ██   ████",
  "█  █ █  █ █ █ █ █  █   █  █ █  █  █    █  █ █ █  █  █",
  "███  █  █ █████ █  █    ██  █████ ████ █  █ █  █ █  █",
  "                                █"
};
char *BB1[]=
{
  "   █",
  "  ██",
  "   █",
  "   █",
  "   █"
};
char *BB2[]=
{
  " ██",
  "█  █",
  "  █",
  " █",
  "████"
};
char *BB3[]=
{
  " ██",
  "█  █",
  "  █",
  "█  █",
  " ██"
};
char *BB4[]=
{
  "█  █",
  "█  █",
  "████",
  "   █",
  "   █"
};
char *BB5[]=
{
  "████",
  "█",
  "████",
  "   █",
  "████"
};
int   operation[9]={1,0,0,0,1,100,10,0};
char  NAME[55];
char *noname="неизвестно";
char *yesno[2]={"НЕТ","ДА "};
char  symboloperation[4]={"+-*/"};
char *stringoperation[4]={"сложение","вычитание","умножение","деление"};
FILE *F;
char  keys[2];
int   key;
char *cont="Для продолжения нажми клавишу <Tab>.";
char *help[4]=
{
  " <+-*/ИОДКС> - установки, <F1> - результат, <Esc> - выход, <Enter> - старт  ",
  "      <->><<-><Home><End> - курсор, <Enter> - запомнить, <Esc> - выход      ",
  "<->><<-><Home><End> - курсор, <Enter> - ответ, <Esc> - сдаюсь, <F10> - выход",
  " <->><<-><Home><End> - курсор, <Enter> - ввод, <Esc> - сброс, <F10> - выход "
};
struct statement
{
  int  numbers[4];
  int  make;
  int  right;
  int percent;
};
struct ITOG
{
  char name[55];
  int  oper[4];
  int  number;
  int  realnumber;
  int  limits[2];
  int  level;
  int  ball;
  int  opernumber[4];
  int  rightnumber;
  int  rightopernumber[4];
  struct statement S[200];
} itog;
char symbols[3][32]={"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ",
                     "абвгдежзийклмнопрстуфхцчшщъыьэюя",
                     "ABWGDEGZIJKLMNOPRSTUFHCCHH'YJEUA"};
int  Nvisio;
int  scroll;
clock_t oldT,newT,lscr,curT,curI;
int  mmm=10;
int  Mx,My,Mm,Mdx,Mdy,MOUSE,KEYMOUSE;
int  n1,n2,b1,b2,KLMN;
char *nn1,*nn2;

void main(void)
{
  int i,flag;

  b1=80*8-1;
  b2=25*8-1;
  nn1=(char *)&n1;
  nn2=(char *)&n2;
  n1=0x8FF;
  n2=0x2500;
  MOUSE=Initialize(&KEYMOUSE);
  randomize();
  F=fopen("a.dat","rb");
  if(F!=NULL)
  {
    fread(operation,sizeof(int),9,F);
    fread(NAME,55,1,F);
    fclose(F);
  }
  if(NAME[0]==0x0)
  {
    for(i=0; i<55; i++)
      NAME[i]=0x0;
    i=0;
    while(noname[i]!=0x0)
    {
      NAME[i]=noname[i];
      i++;
    }
  }
  _setcursortype(_NOCURSOR);
  Npage(1,1);
  createscreen();
  key=0;
  flag=0;
  if(MOUSE==-1)
  {
    Mdx=1;
    Mdy=1;
    SetTextCursor(0,n1,n2);
    Mm=GetPos(&Mx,&My);
  }
  KLMN=0;
  while(key!=_Esc_)
  {
    KLMN++;
    if((MOUSE==-1)&&(KLMN>5000))
    {
      KLMN=0;
      Mx+=Mdx;
      if(Mx>=b1)
        Mdx=-1;
      if(Mx<=0)
        Mdx=1;
      My+=Mdy;
      if(My>=b2)
        Mdy=-1;
      if(My<=0)
        Mdy=1;
      SetPos(Mx,My);
    }
    key=Getc(&keys[0],&keys[1]);
    if(key!=0)
    {
      switch(key)
      {
        case _F5_ :
          if(nn1[0]>0x0)
          {
            nn1[0]--;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _F6_ :
          if(nn1[0]<0xFF)
          {
            nn1[0]++;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _F7_ :
          if(nn2[0]>0x0)
          {
            nn2[0]--;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _F8_ :
          if(nn2[0]<0xFF)
          {
            nn2[0]++;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _5_ :
          if(nn1[1]>0x0)
          {
            nn1[1]--;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _6_ :
          if(nn1[1]<0xFF)
          {
            nn1[1]++;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _7_ :
          if(nn2[1]>0x0)
          {
            nn2[1]--;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _8_ :
          if(nn2[1]<0xFF)
          {
            nn2[1]++;
            SetTextCursor(0,n1,n2);
          }
          break;
        case _SEQUAL_ :
        case _RPLUS_ :
          operation[0]=1-operation[0];
          if((operation[0]==0)&&(operation[1]==0)&&
             (operation[2]==0)&&(operation[3]==0))
            operation[0]=1-operation[0];
          else
            refreshoperation();
          break;
        case _MINUS_ :
        case _RMINUS_ :
          operation[1]=1-operation[1];
          if((operation[0]==0)&&(operation[1]==0)&&
             (operation[2]==0)&&(operation[3]==0))
            operation[1]=1-operation[1];
          else
            refreshoperation();
          break;
        case _S8_ :
        case _RASTER_ :
          operation[2]=1-operation[2];
          if((operation[0]==0)&&(operation[1]==0)&&
             (operation[2]==0)&&(operation[3]==0))
            operation[2]=1-operation[2];
          else
            refreshoperation();
          break;
        case _SLASH_ :
          operation[3]=1-operation[3];
          if((operation[0]==0)&&(operation[1]==0)&&
             (operation[2]==0)&&(operation[3]==0))
            operation[3]=1-operation[3];
          else
            refreshoperation();
          break;
        case _Enter_ :
          clearbase();
          control();
          createscreen();
          flag=1;
          break;
        case _F1_ :
          if(flag==1)
            visio();
          break;
        default :
          switch(keys[0])
          {
            case 'и' :
            case 'И' :
            case 'b' :
            case 'B' :
              input(25,6,-1,54);
              if(NAME[0]==0x0)
              {
                for(i=0; i<55; i++)
                  NAME[i]=0x0;
                i=0;
                while(noname[i]!=0x0)
                {
                  NAME[i]=noname[i];
                  i++;
                }
              }
              break;
            case 'о' :
            case 'О' :
            case 'j' :
            case 'J' :
              input(54,7,4,5);
              break;
            case 'д' :
            case 'Д' :
            case 'l' :
            case 'L' :
              input(64,7,5,5);
              break;
            case 'к' :
            case 'К' :
            case 'r' :
            case 'R' :
              input(51,8,6,4);
              break;
            case 'с' :
            case 'С' :
            case 'c' :
            case 'C' :
              input(72,8,8,2);
              break;
          }
          break;
      }
    }
  }
  Npage(0,1);
  _setcursortype(_NORMALCURSOR);
  F=fopen("a.dat","wb");
  fwrite(operation,sizeof(int),9,F);
  fwrite(NAME,55,1,F);
  fclose(F);
}

void createscreen(void)
{
  int ii;
  char tmp[10];

  TextColor(YELLOW);
  TextBackGround(BLUE);
  ClrScr();
  TextColor(GREEN);
  CprintXY(30,1,"У С Т Н Ы Й   С Ч Е Т");
  CprintXY(30,2,"==== версия 1.00 ====");
  CprintXY(31,3,"ВАН");
  CprintXY(46,3,"1997");
  TextColor(LIGHTCYAN);
  CprintXY(3,5,"Действие:");
  CprintXY(3,6,"~~~~~~~~~");
  for(ii=0; ii<4; ii++)
  {
    TextColor(YELLOW);
    GetCharXY(7,7+ii,symboloperation[ii]);
    TextColor(LIGHTCYAN);
    CprintXY(9,7+ii,stringoperation[ii]);
  }
  refreshoperation();
  TextColor(LIGHTCYAN);
  TextBackGround(BLUE);
  CprintXY(20,6,"Имя:");
  CprintXY(29,7,"Величина результата:  от        до");
  CprintXY(29,8,"Количество примеров:");
  CprintXY(60,8,"Сложность:");
  CprintXY(36,10,"Оценка:");
  CprintXY(36,11,"~~~~~~~");
  CprintXY(10,12,"Пример:");
  CprintXY(10,13,"~~~~~~~");
  CprintXY(58,12,"Верный ответ:");
  CprintXY(58,13,"~~~~~~~~~~~~~");
  CprintXY(10,14,"Проверка:   а)");
  CprintXY(10,15,"~~~~~~~~~   б)");
  CprintXY(58,14,"Верных ответов:");
  CprintXY(58,15,"~~~~~~~~~~~~~~~");
  TextColor(YELLOW);
  CprintXY(20,6,"И");
  CprintXY(51,7,"о");
  CprintXY(61,7,"д");
  CprintXY(29,8,"К");
  CprintXY(60,8,"С");
  TextColor(RED);
  CprintXY(3,25,help[0]);
  TextColor(LIGHTMAGENTA);
  CprintXY(25,6,NAME);
  sprintf(tmp,"%-5i",operation[4]);
  CprintXY(54,7,tmp);
  sprintf(tmp,"%-5i",operation[5]);
  CprintXY(64,7,tmp);
  sprintf(tmp,"%-4i",operation[6]);
  CprintXY(51,8,tmp);
  sprintf(tmp,"%-2i",operation[8]);
  CprintXY(72,8,tmp);
}

void refreshoperation(void)
{
  int ii;

  TextBackGround(BLUE);
  TextColor(LIGHTMAGENTA);
  for(ii=0; ii<4; ii++)
    CprintXY(2,7+ii,yesno[operation[ii]]);
}

void input(int sx,int sy,int n, int nn)
{
  int  ll,kk,cc,ee,flag;
  char tmp[56],ks[2];
  char cursor[4]="─\\│/";
  double clck,clck_;

  flag=0;
  lscr=(clock_t)(operation[8]+5)*CLK_TCK;
  clck_=clck=(double)lscr/(double)80;
  curI=CLK_TCK/(clock_t)mmm;
  cc=0;
  scroll=0;
  if(n==7)
  {
    for(ll=0; ll<nn; ll++)
      tmp[ll]=' ';
    tmp[nn]=0x0;
    ll=0;
  }
  else
  {
    if(n==-1)
    {
      for(ll=0; ll<nn; ll++)
        tmp[ll]=' ';
      tmp[nn]=0x0;
      ll=0;
      while(NAME[ll]!=0x0)
      {
        tmp[ll]=NAME[ll];
        ll++;
      }
    }
    else
    {
      sprintf(tmp,"%-5i",operation[n]);
      tmp[nn]=0x0;
      ll=0;
      while((tmp[ll]!=' ')&&(tmp[ll]!=0x0))
        ll++;
    }
  }
  TextColor(RED);
  if(n==7)
    CprintXY(3,25,help[3]);
  else
    CprintXY(3,25,help[1]);
  if(n!=-1)
  {
    tmp[nn]=' ';
    tmp[nn+1]=0x0;
  }
  TextColor(LIGHTMAGENTA);
  ee=ll;
  GotoXY(sx+ll,sy);
  kk=0;
  curT=oldT=newT=clock();
  while(kk!=_Esc_)
  {
    newT=clock();
    if((n==7)&&((newT-oldT)>(clock_t)clck))
    {
      scroll++;
      GetCharXY(scroll,24,'█');
      if(scroll>=80)
      {
        operation[7]=-5;
        return;
      }
      clck+=clck_;
    }
    if((newT-curT)>curI)
    {
      curT=newT;
      GetCharXY(sx+ll,sy,cursor[cc]);
      cc++;
      if(cc>3)
        cc=0;
    }
    KLMN++;
    if((MOUSE==-1)&&(KLMN>5000))
    {
      KLMN=0;
      Mx+=Mdx;
      if(Mx>=b1)
        Mdx=-1;
      if(Mx<=0)
        Mdx=1;
      My+=Mdy;
      if(My>=b2)
        Mdy=-1;
      if(My<=0)
        Mdy=1;
      SetPos(Mx,My);
    }
    kk=Getc(&ks[0],&ks[1]);
    if(kk!=0)
    {
      switch(kk)
      {
        case _RPLUS_ :
          if((clock_t)mmm<CLK_TCK)
          {
            mmm++;
            curI=CLK_TCK/(clock_t)mmm;
          }
          else
            errorsound();
          break;
        case _RMINUS_ :
          if(mmm>1)
          {
            mmm--;
            curI=CLK_TCK/(clock_t)mmm;
          }
          else
            errorsound();
          break;
        case _0_ :
        case _1_ :
        case _2_ :
        case _3_ :
        case _4_ :
        case _5_ :
        case _6_ :
        case _7_ :
        case _8_ :
        case _9_ :
        case _R0_ :
        case _R1_ :
        case _R2_ :
        case _R3_ :
        case _R4_ :
        case _R5_ :
        case _R6_ :
        case _R7_ :
        case _R8_ :
        case _R9_ :
          if(flag==0)
          {
            for(ll=0; ll<=nn; ll++)
              tmp[ll]=' ';
            tmp[nn+1]=0x0;
            ll=0;
            CprintXY(sx,sy,tmp);
          }
          if(ll<nn)
          {
            tmp[ll]=ks[0];
            CprintXY(sx,sy,tmp);
            ll++;
            if(ll>ee)
              ee=ll;
          }
          flag=1;
          break;
        case _BackSpace_ :
          if(ll>0)
          {
            if(ll==ee)
              ee--;
            ll--;
            tmp[ll]=' ';
            CprintXY(sx,sy,tmp);
            while((ee>0)&&(tmp[ee-1]==' '))
              ee--;
          }
          flag=1;
          break;
        case _Space_ :
          if(n==-1)
          {
            if(flag==0)
            {
              for(ll=0; ll<=nn; ll++)
                tmp[ll]=' ';
              tmp[nn+1]=0x0;
              ll=0;
              CprintXY(sx,sy,tmp);
            }
            if(ll<nn)
            {
              tmp[ll]=ks[0];
              CprintXY(sx,sy,tmp);
              ll++;
              if(ll>ee)
                ee=ll;
            }
            flag=1;
            break;
          }
        case _Enter_ :
          if(n==-1)
          {
            for(ll=0; ll<55; ll++)
              NAME[ll]=tmp[ll];
            NAME[54]=0x0;
            ll=54;
            while((NAME[ll]==' ')||(NAME[ll]==0x0))
            {
              NAME[ll]=0x0;
              ll--;
            }
          }
          else
            operation[n]=atoi(tmp);
          kk=_Esc_;
          break;
        case _Left_ :
          if(ll>0)
          {
            ll--;
            CprintXY(sx,sy,tmp);
          }
          flag=1;
          break;
        case _Right_ :
          if(ll<nn)
          {
            ll++;
            CprintXY(sx,sy,tmp);
          }
          flag=1;
          break;
        case _Home_ :
          if(ll!=0)
          {
            ll=0;
            CprintXY(sx,sy,tmp);
          }
          flag=1;
          break;
        case _End_ :
          if(ll!=ee)
          {
            ll=ee;
            CprintXY(sx,sy,tmp);
          }
          flag=1;
          break;
        case _Esc_ :
          if(n==7)
          {
            operation[7]=-7;
            return;
          }
          break;
        case _F10_ :
          if(n==7)
          {
            operation[7]=-3;
            return;
          }
          break;
        default :
          if(n==-1)
          {
            if(flag==0)
            {
              for(ll=0; ll<=nn; ll++)
                tmp[ll]=' ';
              tmp[nn+1]=0x0;
              ll=0;
              CprintXY(sx,sy,tmp);
            }
            if(ll<nn)
            {
              tmp[ll]=ks[0];
              CprintXY(sx,sy,tmp);
              ll++;
              if(ll>ee)
                ee=ll;
            }
            flag=1;
          }
          break;
      }
    }
  }
  if(n==-1)
  {
    CprintXY(sx,sy,NAME);
    CprintXY(80,sy," ");
    ll=54;
    while(NAME[ll]==' ')
    {
      NAME[ll]=0x0;
      ll--;
    }
    for(ll=0; ll<55; ll++)
      tmp[ll]=' ';
    tmp[55]=0x0;
    CprintXY(sx,sy,tmp);
    CprintXY(sx,sy,NAME);
  }
  else
  {
    switch(n)
    {
      case 4 :
        if((operation[5]-operation[4])<50)
          operation[4]=operation[5]-50;
        break;
      case 5 :
        if((operation[5]-operation[4])<50)
          operation[5]=operation[4]+50;
        break;
      case 6 :
        if(operation[6]<5)
          operation[6]=5;
        if(operation[6]>200)
          operation[6]=200;
        break;
    }
    if(nn==5)
    {
      sprintf(tmp,"%-6i",operation[n]);
      CprintXY(sx,sy,tmp);
    }
    else
    {
      sprintf(tmp,"%-5i",operation[n]);
      CprintXY(sx,sy,tmp);
    }
    if(n!=7)
    {
      TextColor(RED);
      CprintXY(3,25,help[0]);
    }
  }
}

void control(void)
{
  int  ii,ll,kk,mm,cc,number,OR;
  char tmp[10];
  char tmps[30];
  int  arg1,arg2,result,right,ball,n;
  double percent;

  TextColor(RED);
  CprintXY(3,25,help[2]);
  n=0;
  right=0;
  for(ii=0; ii<4; ii++)
    if(operation[ii]==1)
      n++;
  number=0;
  while(operation[number]==0)
    number++;
  for(ii=0; ii<operation[6]; ii++)
  {
    for(ll=0; ll<29; ll++)
      tmps[ll]=0x0;
    clearplace();
    if(n!=1)
    {
      number=random(n);
      ll=-1;
      kk=-1;
      while(ll!=number)
      {
        kk++;
        if(operation[kk]==1)
          ll++;
      }
      number=kk;
    }
    switch(number)
    {
      case 0 :
newadd:
        result=random(operation[5]-operation[4])+operation[4];
        arg1=random(result);
        arg2=result-arg1;
        if((arg1==0)||(arg2==0)||(result==0))
          goto newadd;
        break;
      case 1 :
newsub:
        arg1=random(operation[5]-operation[4])+operation[4];
        arg2=random(arg1);
        result=arg1-arg2;
        if((arg1==0)||(arg2==0)||(result==0))
          goto newsub;
        break;
      case 2 :
newresult:
        result=random(operation[5]-operation[4])+operation[4];
        arg1=random(result);
        if(arg1==0)
          arg1=1;
        arg2=result/arg1;
        cc=arg1;
        ll=1;
        kk=arg1*arg2;
        mm=1;
        while(kk!=result)
        {
          if(((arg1+ll)>result)&&((arg1-ll)<0))
            goto newresult;
          cc=arg1+ll*mm;
          if(cc==0)
            cc=1;
          arg2=result/cc;
          kk=cc*arg2;
          if(mm==1)
            mm=-1;
          else
          {
            mm=1;
            ll++;
          }
        }
        arg1=cc;
        if((arg1==1)||(arg2==1)||(result==1))
          goto newresult;
        break;
      case 3 :
newarg1:
        arg1=random(operation[5]-operation[4])+operation[4];
        arg2=random(arg1);
        if(arg2==0)
          arg2=1;
        result=arg1/arg2;
        cc=arg2;
        ll=1;
        kk=result*arg2;
        mm=1;
        while(kk!=arg1)
        {
          if(((arg2+ll)>arg1)&&((arg2-ll)<0))
            goto newarg1;
          cc=arg2+ll*mm;
          if(cc==0)
            cc=1;
          result=arg1/cc;
          kk=result*cc;
          if(mm==1)
            mm=-1;
          else
          {
            mm=1;
            ll++;
          }
        }
        arg2=cc;
        if((arg1==1)||(arg2==1)||(result==1))
          goto newarg1;
        break;
    }
    sprintf(tmps,"%i %c %i = ",arg1,symboloperation[number],arg2);
    ll=strlen(tmps);
    TextColor(LIGHTMAGENTA);
    CprintXY(27,12,tmps);
    sprintf(tmp,"%3i",ii+1);
    CprintXY(17,12,tmp);
    operation[7]=-1;
    input(27+ll,12,7,5);
    TextColor(RED);
    CprintXY(3,25,help[2]);
    itog.realnumber++;
    itog.opernumber[number]++;
    itog.S[ii].numbers[0]=arg1;
    itog.S[ii].numbers[1]=arg2;
    itog.S[ii].numbers[2]=result;
    itog.S[ii].numbers[3]=operation[7];
    itog.S[ii].make=number;
    itog.S[ii].right=0;
    itog.S[ii].percent=(int)((newT-oldT)/CLK_TCK);
    if(operation[7]==result)
    {
      right++;
      itog.rightopernumber[number]++;
      itog.S[ii].right=1;
    }
    percent=(double)right*100.0/(double)(ii+1);
    if(percent>=98.0)
      ball=5;
    else
      if(percent>=75.0)
        ball=4;
      else
        if(percent>=50.0)
          ball=3;
        else
          if(percent>=20.0)
            ball=2;
          else
            ball=1;
    TextColor(LIGHTMAGENTA);
    sprintf(tmp,"%1i",ball);
    CprintXY(45,10,tmp);
    if(operation[7]>0)
    {
      TextColor(YELLOW);
      switch(number)
      {
        case 0 :
          sprintf(tmps,"%i - %i = %i",operation[7],arg1,operation[7]-arg1);
          CprintXY(27,14,tmps);
          sprintf(tmps,"%i - %i = %i",operation[7],arg2,operation[7]-arg2);
          CprintXY(27,15,tmps);
          break;
        case 1 :
          sprintf(tmps,"%i - %i = %i",arg1,operation[7],arg1-operation[7]);
          CprintXY(27,14,tmps);
          sprintf(tmps,"%i + %i = %i",arg2,operation[7],arg2+operation[7]);
          CprintXY(27,15,tmps);
          break;
        case 2 :
          ll=operation[7]/arg1;
          kk=ll*arg1;
          mm=operation[7]-kk;
          if(mm==0)
            sprintf(tmps,"%i / %i = %i",operation[7],arg1,ll);
          else
            sprintf(tmps,"%i / %i = %i (остаток %i)",operation[7],arg1,ll,mm);
          CprintXY(27,14,tmps);
          ll=operation[7]/arg2;
          kk=ll*arg2;
          mm=operation[7]-kk;
          if(mm==0)
            sprintf(tmps,"%i / %i = %i",operation[7],arg2,ll);
          else
            sprintf(tmps,"%i / %i = %i (остаток %i)",operation[7],arg2,ll,mm);
          CprintXY(27,15,tmps);
          break;
        case 3 :
          ll=arg1/operation[7];
          kk=ll*operation[7];
          mm=arg1-kk;
          if(mm==0)
            sprintf(tmps,"%i / %i = %i",arg1,operation[7],ll);
          else
            sprintf(tmps,"%i / %i = %i (остаток %i)",arg1,operation[7],ll,mm);
          CprintXY(27,14,tmps);
          sprintf(tmps,"%i * %i = %i",arg2,operation[7],arg2*operation[7]);
          CprintXY(27,15,tmps);
          break;
      }
      TextColor(LIGHTGREEN);
      if(operation[7]==result)
      {
        for(kk=0; kk<6; kk++)
          CprintXY(14,17+kk,RR[kk]);
      }
      else
      {
        for(kk=0; kk<5; kk++)
          CprintXY(11,17+kk,LL[kk]);
      }
      OR=outresult(result,right);
      if(OR==_F10_)
        goto end;
    }
    else
    {
      if(operation[7]==-3)
      {
        goto end;
      }
      else
      {
        if((operation[7]==-5)||(operation[7]==-5))
        {
          for(kk=0; kk<5; kk++)
            CprintXY(12,17+kk,TT[kk]);
          OR=outresult(result,right);
          if(OR==_F10_)
            goto end;
        }
      }
    }
  }
end:
  clearplace();
  for(kk=0; kk<6; kk++)
    CprintXY(10,17+kk,EE[kk]);
  switch(ball)
  {
    case 1 :
      for(kk=0; kk<5; kk++)
        CprintXY(66,17+kk,BB1[kk]);
      break;
    case 2 :
      for(kk=0; kk<5; kk++)
        CprintXY(66,17+kk,BB2[kk]);
      break;
    case 3 :
      for(kk=0; kk<5; kk++)
        CprintXY(66,17+kk,BB3[kk]);
      break;
    case 4 :
      for(kk=0; kk<5; kk++)
        CprintXY(66,17+kk,BB4[kk]);
      break;
    case 5 :
      for(kk=0; kk<5; kk++)
        CprintXY(66,17+kk,BB5[kk]);
      break;
  }
  itog.ball=ball;
  itog.rightnumber=right;
  CprintXY(23,23,cont);
  ii=Getc(&tmp[0],&tmp[1]);
  while(ii!=_Tab_)
  {
    KLMN++;
    if((MOUSE==-1)&&(KLMN>5000))
    {
      KLMN=0;
      Mx+=Mdx;
      if(Mx>=b1)
        Mdx=-1;
      if(Mx<=0)
        Mdx=1;
      My+=Mdy;
      if(My>=b2)
        Mdy=-1;
      if(My<=0)
        Mdy=1;
      SetPos(Mx,My);
    }
    ii=Getc(&tmp[0],&tmp[1]);
  }
  savebase();
  TextColor(RED);
  CprintXY(3,25,help[0]);
}

void clearplace(void)
{
  int ll;
  char tmps[30];

  for(ll=0; ll<29; ll++)
    tmps[ll]=' ';
  tmps[29]=0x0;
  TextColor(LIGHTCYAN);
  TextBackGround(BLUE);
  CprintXY(27,12,tmps);
  CprintXY(27,14,tmps);
  CprintXY(27,15,tmps);
  for(ll=0; ll<8; ll++)
  {
    CprintXY(1,17+ll,tmps);
    CprintXY(21,17+ll,tmps);
    CprintXY(41,17+ll,tmps);
    CprintXY(52,17+ll,tmps);
  }
}

int outresult(int result,int right)
{
  char tmp[10];
  int  tmpOR;

  CprintXY(23,23,cont);
  TextColor(LIGHTMAGENTA);
  sprintf(tmp,"%-3i",result);
  CprintXY(73,12,tmp);
  sprintf(tmp,"%-3i",right);
  CprintXY(74,14,tmp);
  tmpOR=0;
  while((tmpOR!=_Tab_)&&(tmpOR!=_F10_))
  {
    KLMN++;
    if((MOUSE==-1)&&(KLMN>5000))
    {
      KLMN=0;
      Mx+=Mdx;
      if(Mx>=b1)
        Mdx=-1;
      if(Mx<=0)
        Mdx=1;
      My+=Mdy;
      if(My>=b2)
        Mdy=-1;
      if(My<=0)
        Mdy=1;
      SetPos(Mx,My);
    }
    tmpOR=Getc(&tmp[0],&tmp[1]);
  }
  return(tmpOR);
}

void clearbase(void)
{
  int i,j;

  for(i=0; i<55; i++)
    itog.name[i]=NAME[i];
  for(i=0; i<4; i++)
  {
    itog.oper[i]=operation[i];
    itog.opernumber[i]=0;
    itog.rightopernumber[i]=0;
  }
  itog.number=operation[6];
  itog.realnumber=0;
  itog.limits[0]=operation[4];
  itog.limits[1]=operation[5];
  itog.level=operation[8];
  itog.ball=0;
  itog.rightnumber=0;
  for(i=0; i<200; i++)
  {
    for(j=0; j<4; j++)
      itog.S[i].numbers[j]=0;
    itog.S[i].make=-10;
    itog.S[i].right=-10;
    itog.S[i].percent=100;
  }
}

void savebase(void)
{
  int  i,j,k,l,m;
  FILE *FILE;
  char filename[13]="________.000";
  struct ftime FT,FTnew;
  struct date D;
  struct time T;
  char tmp[20];

  for(i=0; i<8; i++)
    filename[i]='_';
  filename[8]='.';
  for(i=9; i<12; i++)
    filename[i]='0';
  filename[12]=0x0;
  i=0;
  for(k=0; k<7; k++)
  {
    if(NAME[k]==0x0)
      break;
    for(j=0; j<32; j++)
      if((NAME[k]==symbols[0][j])||(NAME[k]==symbols[1][j]))
      {
        if((NAME[k]=='ш')||(NAME[k]=='Ш')||(NAME[k]=='щ')||(NAME[k]=='щ'))
        {
          filename[i]='S';
          i++;
          if(i>6)
            break;
          filename[i]='H';
          i++;
        }
        else
        {
          if((NAME[k]=='ч')||(NAME[k]=='Ч'))
          {
            filename[i]='C';
            i++;
            if(i>6)
              break;
            filename[i]='H';
            i++;
          }
          else
          {
            filename[i]=symbols[2][j];
            i++;
            break;
          }
        }
      }
    if(i>6)
      break;
  }
  filename[8]='.';
  j=0;
  k=0;
  sprintf(&filename[9],"%03i",j);
  FILE=NULL;
  FILE=fopen(filename,"rb");
  getftime(fileno(FILE),&FT);
  while(FILE!=NULL)
  {
    getftime(fileno(FILE),&FTnew);
    l=0;
    if(FTnew.ft_year<FT.ft_year)
      l=1;
    else
    {
      if(FTnew.ft_year==FT.ft_year)
      {
        if(FTnew.ft_month<FT.ft_month)
          l=1;
        else
        {
          if(FTnew.ft_month==FT.ft_month)
          {
            if(FTnew.ft_day<FT.ft_day)
              l=1;
            else
            {
              if(FTnew.ft_day==FT.ft_day)
              {
                if(FTnew.ft_hour<FT.ft_hour)
                  l=1;
                else
                {
                  if(FTnew.ft_hour==FT.ft_hour)
                  {
                    if(FTnew.ft_min<FT.ft_min)
                      l=1;
                    else
                    {
                      if(FTnew.ft_min==FT.ft_min)
                      {
                        if(FTnew.ft_tsec<FT.ft_tsec)
                          l=1;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    if(l==1)
    {
      getftime(fileno(FILE),&FT);
      k=j;
    }
    fclose(FILE);
    j++;
    if(j>999)
    {
      sprintf(&filename[9],"%03i",k);
      FILE=NULL;
    }
    else
    {
      sprintf(&filename[9],"%03i",j);
      FILE=NULL;
      FILE=fopen(filename,"rb");
    }
  }
  FILE=fopen(filename,"wb");
  getdate(&D);
  gettime(&T);
  fprintf(FILE,"Имя: %s     дата: %2i.%02i.%02i  время: %2i:%02i:%02i\r\n",
                itog.name,ASCII(D.da_day),ASCII(D.da_mon),D.da_year,
                ASCII(T.ti_hour),ASCII(T.ti_min),ASCII(T.ti_sec));
  fprintf(FILE,"Действия: ");
  l=0;
  for(i=0; i<4; i++)
  {
    if(itog.oper[i]==1)
    {
      if(l==1)
        fprintf(FILE,",");
      fprintf(FILE," %s",stringoperation[i]);
      l=1;
    }
  }
  fprintf(FILE,"\r\nКоличество: %i,  пределы: от %i до %i,  сложность: %i\r\n",
                itog.number,itog.limits[0],itog.limits[1],itog.level);
  fprintf(FILE,"Верных ответов: %i,  оценка: %i\r\n",itog.rightnumber,itog.ball);
  fprintf(FILE,"Количество действий: ");
  l=0;
  for(i=0; i<4; i++)
  {
    if(itog.oper[i]==1)
    {
      if(l==1)
        fprintf(FILE,"                     ");
      fprintf(FILE,"%9s - %i, верных ответов - %i\r\n",stringoperation[i],
              itog.opernumber[i],itog.rightopernumber[i]);
      l=1;
    }
  }
  fprintf(FILE,"Предложенные действия:\r\n");
  l=0;
  while((itog.S[l].make!=-10)||(itog.S[l].right!=-10))
  {
    fprintf(FILE,"%3i. ",l+1);
    if(itog.S[l].right==0)
      fprintf(FILE,"не");
    else
      fprintf(FILE,"  ");
    sprintf(tmp,"%i%c%i=%i",itog.S[l].numbers[0],
                 symboloperation[itog.S[l].make],itog.S[l].numbers[1],
                 itog.S[l].numbers[2]);
    fprintf(FILE,"верно:   %-18s",tmp);
    switch(itog.S[l].numbers[3])
    {
      case -7 :
        fprintf(FILE,"нажата клавиша <Esc>");
        break;
      case -5 :
        fprintf(FILE,"вышло время         ");
        break;
      case -3 :
        fprintf(FILE,"нажата клавиша <F10>");
        break;
      default :
        fprintf(FILE,"введено %-5i       ",itog.S[l].numbers[3]);
        break;
    }
    sprintf(tmp,"  прошло %3i секунд\r\n",itog.S[l].percent);
    m=itog.S[l].percent-(itog.S[l].percent/10)*10;
    if((itog.S[l].percent>10)&&(itog.S[l].percent<20))
      m=0;
    switch(m)
    {
      case 0 :
      case 5 :
      case 6 :
      case 7 :
      case 8 :
      case 9 :
        sprintf(tmp,"  прошло %3i секунд\r\n",itog.S[l].percent);
        break;
      case 1 :
        sprintf(tmp,"  прошла %3i секунда\r\n",itog.S[l].percent);
        break;
      case 2 :
      case 3 :
      case 4 :
        sprintf(tmp,"  прошло %3i секунды\r\n",itog.S[l].percent);
        break;
    }
    fprintf(FILE,tmp);
    l++;
  }
  fclose(FILE);
}

void visio(void)
{
  int  kk;
  char ks[2];

  Npage(2,1);
  Nvisio=0;
  createvisio();
  refreshvisio();
  kk=0;
  while(kk!=_Esc_)
  {
    KLMN++;
    if((MOUSE==-1)&&(KLMN>5000))
    {
      KLMN=0;
      Mx+=Mdx;
      if(Mx>=b1)
        Mdx=-1;
      if(Mx<=0)
        Mdx=1;
      My+=Mdy;
      if(My>=b2)
        Mdy=-1;
      if(My<=0)
        Mdy=1;
      SetPos(Mx,My);
    }
    kk=Getc(&ks[0],&ks[1]);
    if(kk!=0)
    {
      switch(kk)
      {
        case _Up_ :
          if(Nvisio!=0)
          {
            Nvisio--;
            refreshvisio();
          }
          break;
        case _Down_ :
          if((Nvisio+13)<itog.realnumber)
          {
            Nvisio++;
            refreshvisio();
          }
          break;
        case _PageUp_ :
          if(Nvisio!=0)
          {
            Nvisio-=12;
            if(Nvisio<0)
              Nvisio=0;
            refreshvisio();
          }
          break;
        case _PageDown_ :
          if((Nvisio+13)<itog.realnumber)
          {
            Nvisio+=12;
            if((Nvisio+13)>itog.realnumber)
              Nvisio=itog.realnumber-13;
            refreshvisio();
          }
          break;
        case _Home_ :
          if(Nvisio!=0)
          {
            Nvisio=0;
            refreshvisio();
          }
          break;
        case _End_ :
          if((Nvisio+13)<itog.realnumber)
          {
            Nvisio=itog.realnumber-13;
            refreshvisio();
          }
          break;
      }
    }
  }
  Npage(1,1);
}

void createvisio(void)
{
  char tmp[50];
  int i,l;

  TextColor(YELLOW);
  TextBackGround(BLUE);
  ClrScr();
  TextColor(GREEN);
  CprintXY(31,1,"Р Е З У Л Ь Т А Т Ы");
  CprintXY(31,2,"===================");
  TextColor(LIGHTCYAN);
  CprintXY(1,3,"Имя:");
  CprintXY(1,4,"Действия:");
  CprintXY(1,5,"Количество:");
  CprintXY(18,5,"пределы: от");
  CprintXY(36,5,"до");
  CprintXY(46,5,"сложность:");
  CprintXY(1,6,"Верных ответов:");
  CprintXY(22,6,"оценка:");
  CprintXY(1,7,"Количество действий:");
  CprintXY(1,11,"Предложенные действия:");
  TextColor(LIGHTMAGENTA);
  CprintXY(6,3,itog.name);
  l=11;
  for(i=0; i<4; i++)
  {
    if(itog.oper[i]==1)
    {
      CprintXY(l,4,stringoperation[i]);
      l=l+strlen(stringoperation[i])+1;
    }
  }
  sprintf(tmp,"%i",itog.number);
  CprintXY(13,5,tmp);
  sprintf(tmp,"%i",itog.limits[0]);
  CprintXY(30,5,tmp);
  sprintf(tmp,"%i",itog.limits[1]);
  CprintXY(39,5,tmp);
  sprintf(tmp,"%i",itog.level);
  CprintXY(57,5,tmp);
  sprintf(tmp,"%i",itog.rightnumber);
  CprintXY(17,6,tmp);
  sprintf(tmp,"%i",itog.ball);
  CprintXY(30,6,tmp);
  l=7;
  for(i=0; i<4; i++)
  {
    if(itog.oper[i]==1)
    {
      TextColor(LIGHTCYAN);
      sprintf(tmp,"%9s -",stringoperation[i]);
      CprintXY(22,l,tmp);
      CprintXY(39,l,"верных ответов -");
      TextColor(LIGHTMAGENTA);
      sprintf(tmp,"%i",itog.opernumber[i]);
      CprintXY(35,l,tmp);
      sprintf(tmp,"%i",itog.rightopernumber[i]);
      CprintXY(56,l,tmp);
      l++;
    }
  }
}

void refreshvisio(void)
{
  int i,l,m;
  char tmp[80];

  TextBackGround(BLUE);
  for(l=0; l<13; l++)
  {
    for(i=0; i<79; i++)
      tmp[i]=' ';
    tmp[79]=0x0;
    CprintXY(1,l+12,tmp);
    if((itog.S[l+Nvisio].make==-10)||(itog.S[l+Nvisio].right==-10))
      break;
    TextColor(LIGHTMAGENTA);
    sprintf(tmp,"%3i.",l+Nvisio+1);
    CprintXY(1,l+12,tmp);
    if(itog.S[l+Nvisio].right==0)
      CprintXY(7,l+12,"не");
    CprintXY(10,l+12,"верно:");
    TextColor(LIGHTCYAN);
    sprintf(tmp,"%i%c%i=%i",itog.S[l+Nvisio].numbers[0],
                 symboloperation[itog.S[l+Nvisio].make],itog.S[l+Nvisio].numbers[1],
                 itog.S[l+Nvisio].numbers[2]);
    CprintXY(17,l+12,tmp);
    switch(itog.S[l+Nvisio].numbers[3])
    {
      case -7 :
        sprintf(tmp,"нажата клавиша <Esc>");
        break;
      case -5 :
        sprintf(tmp,"вышло время");
        break;
      case -3 :
        sprintf(tmp,"нажата клавиша <F10>");
        break;
      default :
        sprintf(tmp,"введено %i",itog.S[l+Nvisio].numbers[3]);
        break;
    }
    TextColor(LIGHTMAGENTA);
    CprintXY(30,l+12,tmp);
    m=itog.S[l+Nvisio].percent-(itog.S[l+Nvisio].percent/10)*10;
    if((itog.S[l+Nvisio].percent>10)&&(itog.S[l+Nvisio].percent<20))
      m=0;
    switch(m)
    {
      case 0 :
      case 5 :
      case 6 :
      case 7 :
      case 8 :
      case 9 :
        sprintf(tmp,"  прошло %3i секунд\r\n",itog.S[l+Nvisio].percent);
        break;
      case 1 :
        sprintf(tmp,"  прошла %3i секунда\r\n",itog.S[l+Nvisio].percent);
        break;
      case 2 :
      case 3 :
      case 4 :
        sprintf(tmp,"  прошло %3i секунды\r\n",itog.S[l+Nvisio].percent);
        break;
    }
    CprintXY(52,l+12,tmp);
  }
  TextColor(YELLOW);
  tmp[0]='е';
  tmp[1]='щ';
  tmp[2]='е';
  tmp[3]=0x0;
  tmp[4]=' ';
  tmp[5]=' ';
  tmp[6]=' ';
  tmp[7]=0x0;
  if(Nvisio>0)
    CprintXY(50,11,tmp);
  else
    CprintXY(50,11,&tmp[4]);
  if((Nvisio+13)<itog.realnumber)
    CprintXY(50,25,tmp);
  else
    CprintXY(50,25,&tmp[4]);
}

void errorsound(void)
{
  sound(777);
  delay(300);
  nosound();
}