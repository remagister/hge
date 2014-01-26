// program representing basic physics rules

#include "items.h"

#include <string>
using std::string;
string TITLE = "PhysBall v0.95";		// title text //
HGE *hge = 0; // main hge class

float f = 560, c = 20, r = 750 , l = 50; // world borders // please do not try this at home

hgeResourceManager *res; // resources
hgeFont *Font;


string BallSpr[] = {"ballusual", "ballsoccer"};
int BallSpr_ch= 1; // choose one from ^

World worldbase(f,c,l,r);

// Global variables ========
float g = 300;	// gravity constant
float dt = 0.03;	// timer delta frame

Mouse cursor;
GUI gui;

// event functions

void Start();
void Exit();
void _Settings();

bool Frame();

GuiItem *p=0;
bool MenuRender();
bool MainMenu(){
	hge->Input_GetMousePos(&cursor.X, &cursor.Y);
	cursor.Update(dt);
	hge->Music_Play(worldbase.getMusic(0), true);
	if(hge->Input_KeyUp(HGEK_LBUTTON) && p != 0){
		if(p->getState() == PRESSED){ p->Release(cursor.X,cursor.Y); p=0;}
		return false;
	}
	if(hge->Input_GetKey() == HGEK_LBUTTON){
		p = gui.Click(cursor.X, cursor.Y);
		if(p && p->getState() != PRESSED) {
			p->Press(hge, res);
		}
		return false;
	}

	if(hge->Input_GetKey() == HGEK_ESCAPE){
		Start();
	}
	return false;
}

bool Sets(){

	if(hge->Input_GetKey() == HGEK_ESCAPE) {
		hge->System_SetState(HGE_FRAMEFUNC,MainMenu);
		hge->System_SetState(HGE_RENDERFUNC, MenuRender);
		return false;
	}

	return false;
}
bool SetsRender(){
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	Font->SetScale(32.0);
	Font->SetColor(ARGB(0,255,255,255));
	Font->Render(100, 100, 8, "Coming soon.. :)" );
	hge->Gfx_EndScene();

	return false;
}

bool MenuRender(){
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	gui.Draw();
	if(p) p->Draw();

	hge->Gfx_EndScene();
	return false;
}

Ball *ball;
bool Frame(){

	// waiting for escape
	if(hge->Input_GetKey() == HGEK_ESCAPE) {
		hge->System_SetState(HGE_FRAMEFUNC,MainMenu);
		hge->System_SetState(HGE_RENDERFUNC, MenuRender);
		return false;
	}
	hge->Input_GetMousePos(&cursor.X, &cursor.Y);
	cursor.Update(dt);

	worldbase.UpdateAll(dt,g,hge);
	if(worldbase.isPushed()){
		ball = worldbase.findPushed();
		if(hge->Input_KeyUp(HGEK_LBUTTON)) { ball->Release(dt); return false;}
		ball->Drag(cursor);
		return false;
	}
	if(hge->Input_GetKey() == HGEK_LBUTTON){
		ball = cursor.Click(worldbase);
		if(ball) ball->Push();
		return false;
	}

	worldbase.Collide();

	return false;

}
bool Render(){
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	worldbase.Draw();

	hge->Gfx_EndScene();

	return false;
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int){


	hge = hgeCreate(HGE_VERSION); // allocates memory, creates hge interface, ver 0x180
	hge->System_SetState(HGE_TITLE, TITLE.c_str()); // set title obviously
	hge->System_SetState(HGE_WINDOWED, true); // set windowed app
	hge->System_SetState(HGE_FRAMEFUNC, MainMenu);
	hge->System_SetState(HGE_RENDERFUNC, MenuRender); //actually not needed
	hge->System_SetState(HGE_USESOUND, true); // sound
	hge->System_SetState(HGE_FPS, HGEFPS_VSYNC); // probably 60 fps max?
	hge->System_SetState(HGE_HIDEMOUSE, false);		// dont hide cursor

	if(hge->System_Initiate()){
		res = new hgeResourceManager("resource.res");
		Ball b(res->GetSprite(BallSpr[BallSpr_ch].c_str()),Point(100,100),20,0.5);
		b.AddEffect(res->GetEffect("bounce1"));
		worldbase.AddBall(b);
		b.Set(200,200);
		worldbase.AddBall(b);
		b.Set(300,300);
		worldbase.AddBall(b);
		b.Set(400,400);
		worldbase.AddBall(b);
		Font = res->GetFont("MainFont");
		worldbase.AddMusic(res->GetMusic("music1"));
		worldbase.AddMusic(res->GetMusic("music2"));
		gui.Add(Button(res->GetSprite("button_start_normal"), res->GetSprite("button_start_pushed"), 300, 150, 200, 40, Start)); 
		gui.Add(Button(res->GetSprite("button_settings_normal"),res->GetSprite("button_settings_pushed"),300, 200, 200, 40, _Settings));
		gui.Add(Button(res->GetSprite("button_exit_normal"), res->GetSprite("button_exit_pushed"),300, 250, 200, 40, Exit));
		worldbase.SetBg(res->GetSprite("background"));
		hge->System_Start();
	}
//	 else MessageBox(0, hge->System_GetErrorMessage(), "Error!", MB_OK | MB_APPLMODAL | MB_ICONERROR);

	delete res;
	hge->System_Shutdown();
	hge->Release();
}

bool _Exit(){
	return true;
}

void Start(){
	hge->System_SetState(HGE_FRAMEFUNC, Frame);
	hge->System_SetState(HGE_RENDERFUNC, Render);
}
void Exit(){
	hge->System_SetState(HGE_FRAMEFUNC, _Exit);
}
void _Settings(){
	//hge->System_SetState(HGE_FRAMEFUNC, Sets);
	//hge->System_SetState(HGE_RENDERFUNC, SetsRender);
}