Texture bu
{
 filename=Res\Sprite\ball_usual.png
 resgroup=1
}
Texture bg{
	filename=Res\Sprite\backgroundnew.jpg
}

Sprite background{
	texture=bg
	rect = 1,1,479,359
	blendmode=COLORMUL,ALPHABLEND,NOZWRITE
}

Sprite ballusual
{
 texture=bu
 rect=1, 1, 49, 49
 blendmode=COLORMUL,ALPHABLEND,NOZWRITE
 resgroup=1
}

Texture bs
{
 filename=Res\Sprite\ball_soccer.png
 resgroup=2
}

Sprite ballsoccer
{
 texture=bs
 rect=1,1,63,63
 blendmode=COLORMUL,ALPHABLEND,NOZWRITE
 resgroup=2
}

Font MainFont
{
 filename=Res\Font\font.fnt
 scale=14.0
 tracking=1.0
 color=00FBFBFF
}

Sound bounce1{
	filename=Res\Sound\bounce1.wav
}

Sound bounce2{
	filename=Res\Sound\bounce2.wav
}

Sound buttonclick{
	filename=Res\Sound\button.mp3
}

Texture button_normal{
	filename=Res\Sprite\buttons_normal.bmp
}
Texture button_pushed{
	filename=Res\Sprite\buttons_pressed.bmp
}

Sprite button_exit_normal{
	texture=Res\Sprite\buttons_normal.bmp
	rect=0,0,94,21
}
Sprite button_settings_normal{
	texture=Res\Sprite\buttons_normal.bmp
	rect=0,23,94,21
}
Sprite button_start_normal{
	texture=Res\Sprite\buttons_normal.bmp
	rect=0,46,94,21
}

Sprite button_exit_pushed{
	texture=button_pushed
	rect=0,0,94,21

}
Sprite button_settings_pushed{
	texture=button_pushed
	rect=0,23,94,21
}
Sprite button_start_pushed{
	texture=button_pushed
	rect=0,46,94,21
}