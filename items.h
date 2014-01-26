
#pragma once
#include <cmath>
#include <hge.h>
#include <hgesprite.h>
#include <hgevector.h>
#include <hgefont.h>
#include <hgeresource.h>
#include <vector>

using std::vector;

#define pi 3.14159265358979323846f
#define FRICT 50

float distance(float x1, float y1, float x2, float y2){
	return sqrt(pow(y2- y1, 2) + pow(x2 - x1, 2));
}

// classes ======================

class Point{
public:
	Point(){
		x = y = 0;
	}
	Point(const Point& t) : x(t.x), y(t.y){}
	Point(float X, float Y): x(X), y(Y){}

	void Set(float X, float Y){
		x = X;
		y = Y;
	}
	float DistanceTo(const Point &t){
		return sqrt(pow(x - t.x,2) + pow(y - t.y,2));
	}
	bool OutOfBorders(float x1, float x2, float y1, float y2){
		if(x >= x1 || x <= x2 || y <= y2 || y >= y1) return true;
		else return false;
	}
	bool operator == (const Point &t)const{
		if (x == t.x && y == t.y) return true;
		else return false;
	}
	Point& operator = (const Point& t){
		x = t.x;
		y = t.y;
	}
	// coordinates
	float x, y;
};

float distance(const Point &A, const Point &B){
	return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));
}
float sqdistance(const Point& a, const Point& b){
	return pow((b.x-a.x),2)+pow(b.y - a.y,2);
}

class Vector{
public:		
	// конструкторы
	Vector(): X(0), Y(0){}
	Vector(float x, float y):X(x),Y(y){}
	Vector(Point vec):X(vec.x),Y(vec.y){}
	Vector(float x1, float y1, float x2, float y2):X(x2-x1),Y(y2-y1){}
	Vector(Point begin, Point end): X(end.x-begin.y), Y(end.y-begin.y){}

	// операторы
	Vector operator+ (Vector& right)const{
		return Vector(X+right.X,Y+right.Y);
	}
	Vector& operator+= (Vector& right){
		X += right.X;
		Y += right.Y;
		return *this;
	}
	Vector operator- (Vector& right)const{
		return Vector(X-right.X, Y-right.Y);
	}
	Vector& operator -=(Vector& right){
		X -= right.X;
		Y -= right.Y;
		return *this;
	}
	Vector operator *(float val)const{
		return Vector(X*val, Y*val);
	}
	Vector& operator *= (float val){
		X *= val;
		Y *= val;
		return *this;
	}
	Vector operator /(float val)const{
		return *this*(1/val);
	}
	Vector& operator /= (float val){
		return *this*=(1/val);
	}
	float operator *(Vector& right)const {
		return X*right.X + Y*right.Y;
	}
	float operator /(Vector& right)const{
		return X/right.X + Y/right.Y;
	}

	operator hgeVector(){
		return hgeVector(X,Y);
	}

	// оператор () служит для превращения класса в функтор
	// далее мы можем обходиться с объектом данного класса как с функцией, те "вызывать" скобочками
	// возвращает скалярное произведение на самого себя (или квадрат своей длины)
	float operator()()const{
		return X*X+Y*Y;
	}
	// оператор ~ "дополнение до единицы", в данном случае возвращает тангенс угла наклона к оси ординат
	// отношение Y составляющей к X 
	float operator ~()const{
		return Y/X;
	}
	// оператор приведения типов. трансформирует Vector в Point
	operator Point(){
		return Point(X,Y);
	}

	// отражение вектора от прямой, которая проходит через 2 заданные точки
	void Reflect(Point& a, Point& b){
		Vector n(a.x - b.y, b.x - a.y);
		*this = *this - (n*2)*((*this*n)/ n());
	}
	// поля
	float X,Y;
};


class Mouse_base : public Point{
public:
	// for input_getmouse...
	float X, Y;
	// starts calculating phys properties
	void Update(float delta){
		Set(X, Y);
	}
	
	void Set(float _x, float _y){
		_xl = x;
		_yl = y; 
		x = _x;
		y = _y;
	}



	float DiffX()const{
		return x - _xl;
	}
	float DiffY()const{
		return y - _yl;
	}

private:
	// xlast ylast
	float _xl, _yl;
};


class Ball{
public:
	Ball(){}
	Ball(const hgeSprite* spr, const Point& pos, float Radius, float Bounce, float Mass=0):_center(pos), _size(Radius), _bounce(Bounce),
	_mass(Mass){
		_sprite = new hgeSprite(*spr);
		_pushed = false;
	}
	Ball(const Ball& t): _center(t._center), _size(t._size), _bounce(t._bounce), _mass(t._mass){
		_sprite = new hgeSprite(*t._sprite);
		_pushed = t._pushed;
		bounce = t.bounce;
	}
	// call only in RenderFunc
	void Draw(){
		_sprite->RenderStretch(_center.x-_size, _center.y-_size, _center.x+_size, _center.y+_size);
	}
	void Update(float dt){
		_center.x += _vdir.X*dt;
		_center.y += _vdir.Y*dt;
	}
	void Set(float x, float y){
		_last.Set(_center.x, _center.y);
		_center.Set(x,y);
	}
	void SetV(float dx, float dy){
		_vdir.X = dx;
		_vdir.Y = dy;
	}
	
	void AddEffect(HEFFECT eff){
		bounce = eff;
	}
	void UpdateV(float dx, float dy, float dt){
		_vdir.X += (_vdir.Y == 0? dx*dt - _vdir.X*0.05f  : dx*dt) ;
		_vdir.Y += dy*dt;
	}
	void UpdateV(const Vector& dv, float dt){
		_vdir += dv*dt;
	}
	bool TestCollide(const Ball& b){
		return (sqdistance(_center,b._center) <= pow(b._size + _size,2));
	}
	// detect collision between ball & wall
	bool ProcessWalls(float ceil, float flr, float lw, float rw){
		if((_center.x+ _size >= rw && _vdir.X > 0) || (_center.x- _size <= lw && _vdir.X < 0)) {
			_vdir.X *= -_bounce; 
			return true;
		}
		if(_center.y- _size <= ceil && _vdir.Y < 0) {
			_vdir.Y *= -_bounce;
			_vdir.X *= 0.85f;
			return true;
		}
		if((_center.y+ _size >= flr && _vdir.Y > 0)){
			if(_vdir.Y <= 20.0f){
				_vdir.Y = 0;
				return false;
			}
			_vdir.Y *= -_bounce;
			_vdir.X *= 0.85f;
			return true;
		}
		return false;
	}
	void Drag(const Mouse_base& m){
		Set(_center.x + m.DiffX(), _center.y + m.DiffY());
	}
	bool Contains(float x, float y){
		return (sqdistance(_center, Point(x, y)) <= _size*_size);
	}
	void PlayEffect(HGE* &hge){
		hge->Effect_Play(bounce);
	}
	void Push(){_pushed = true;}
	void Release(float dt) { 
		_pushed = false; 
		_vdir.X = _center.x - _last.x;
		_vdir.Y = _center.y - _last.y;
		_vdir /= dt;
	}
	bool isPushed()const { return _pushed;}
private:
	Point _last;
	Point _center;
	Vector _vdir;
	// coming soon
	float _mass;
	float _bounce;
	float _size;
	hgeSprite *_sprite;
	HEFFECT bounce;
	bool _pushed;
};

class World{
public:

	World(float f, float c, float l, float r): room(c,f,l,r){
	}
	World(const World &t) : room(t.room.x1,t.room.y1,t.room.x2,t.room.y2){
		bg = new hgeSprite(*t.bg);
		
	}
	void AddBall(const Ball &t){
		col.push_back(t);
	}
	void Clear(){
		col.clear();
	}
	void Draw(){
		bg->RenderStretch(0,0,800,600);
		for(int x=0; x<col.size(); x++){
			col[x].Draw();
		}
	}
	void Play(HGE* &hge){
		srand(hge->Timer_GetTime());
		if(musics.empty()) return;
		hge->Music_Play(musics[rand()%musics.size()],true);
	}
	void Delete(int x){
		col.erase(col.begin()+x);
	}
	Ball Pop(){
		Ball ret(col[col.size()]);
		col.pop_back();
		return ret;
	}
	void UpdateAll(float dt, float grav, HGE* &h){
		for(int i=0; i<col.size(); i++){
			if(!col[i].isPushed()){
				col[i].Update(dt);
				col[i].UpdateV(0, grav, dt);
				if(col[i].ProcessWalls(room.x1, room.y1, room.x2, room.y2))
					col[i].PlayEffect(h);
			}
		}
	}
	bool isPushed()const{
		for(int i=0; i<col.size(); i++){
			if(col[i].isPushed()) return true;
		}
		return false;
	}
	Ball* findPushed(){
		for(int i=0; i<col.size(); i++){
			if(col[i].isPushed()) return &col[i];
		}
		return 0;
	}
	Ball& operator[] (int x){
			return col[x];
	}
	
	void Collide(){
		
	}

	void Press(float x, float y){
		for(unsigned i=0 ; i<col.size(); i++){
			if(!col[i].isPushed() && col[i].Contains(x,y)){
				col[i].Push();
				return;
			}
		}
	}
	void Release(float dt){
		for(unsigned i=0 ; i<col.size(); i++){
			if(col[i].isPushed())
				col[i].Release(dt);
		}
	}
	hgeRect getWalls()const{
		return room;
	}
	vector<Ball>& getCollection(){
		return col;
	}
	int getSize()const{
		return col.size();
	}

	void AddMusic(HMUSIC mus){
		musics.push_back(mus);
	}
	void ClearMusic(){
		musics.clear();
	}
	HMUSIC getMusic(int x){
		return musics[x];
	}
	void SetBg(const hgeSprite* b){
		if(bg) delete bg;
		bg = new hgeSprite (*b);
	}
private:
	hgeSprite *bg;
	vector<Ball> col;
	vector<HMUSIC> musics;
	hgeRect room;
};

class Mouse : public Mouse_base{
public:
	Ball* Click(World &wld){
		for(int i=0; i<wld.getSize(); i++){
			if(wld[i].Contains(X,Y))
				return &wld[i];
		}
		return 0;
	}
};

enum ButtonStates { NORMAL = 0, HOVER, PRESSED};

class GuiItem{
public:
	
	virtual void Draw()const = 0;
	virtual void Press(HGE *& env, hgeResourceManager *r) = 0;
	virtual bool HighLight(float x, float y) = 0;
	virtual void Release(float x, float y) = 0;
	virtual ButtonStates getState()const = 0;
};

class Button :public hgeRect, public GuiItem{
public:
	bool _pushed;
	Button(hgeSprite *s, hgeSprite *c, float _x1, float _y1, float w, float h, void (*evn)()) : hgeRect(_x1, _y1, _x1+w, _y1+h){
		norm = new hgeSprite(*s);
		clck = new hgeSprite(*c);
		_pushed = false;
		Event = evn;
	}
	Button() : hgeRect(){
		norm = clck = 0;
		_pushed = false;
	}
	Button(const Button &t){
		hgeRect::Set(t.x1, t.y1,t.x2, t.y2);
		_pushed = t._pushed;
		clck = new hgeSprite(*(t.clck));
		norm = new hgeSprite(*(t.norm));
		Event = t.Event;
	}

	virtual void Draw()const{
		if(_pushed) clck->RenderStretch(x1, y1, x2, y2);
		else norm->RenderStretch(x1, y1, x2, y2);
	}
	bool isPushed() const{
		return _pushed;
	}
	virtual void Press(HGE *& env, hgeResourceManager *r){
		_pushed = true;
		env->Effect_Play(r->GetEffect("buttonclick"));

	}
	virtual void Release(float x, float y){
		if(TestPoint(x,y))
			Event();
		_pushed = false;
	}
	virtual bool HighLight(float x, float y){
		return TestPoint(x,y);
	}
	virtual ButtonStates getState()const{
		return (_pushed? PRESSED : NORMAL);
	}
	void SetEvent(void (*evn)()){
		Event = evn;
	}
	~Button(){
		if(clck) delete clck;
		if (norm) delete norm;
	}
private:
	void (*Event)();
	hgeSprite *clck;
	hgeSprite *norm;
};

class GUI{
protected:
	vector<GuiItem*> col;
	vector<void*> cont;
public:
	GUI(){
	}
	void Add(const Button &t){
		Button* p = new Button(t);
		col.push_back(dynamic_cast<GuiItem*>(p));
		cont.push_back((void*)p);
	}
	void Add(Button *t){
		col.push_back(dynamic_cast<GuiItem*>(t));
		cont.push_back((void*)t);
	}

	GuiItem* Click(float mx, float my){
		for(int a=0; a<col.size(); a++){
			if(col[a]->HighLight(mx,my)){
				return col[a];
			}
			
		}
		return 0;
	}
	void Release(float x, float y){
		for(int a=0; a<col.size(); a++){
			if(col[a]->getState() == PRESSED){
				col[a]->Release(x,y);
				break;
			}
		}
	}
	void Delete(int x){
		delete col[x];
		col.erase(col.begin()+x);
	}

	int Size(){
		return col.size();
	}

	void Draw(){
		for(int a=0;a<col.size(); a++)
			col[a]->Draw();
	}

	~GUI(){
		col.clear();
	}

};
