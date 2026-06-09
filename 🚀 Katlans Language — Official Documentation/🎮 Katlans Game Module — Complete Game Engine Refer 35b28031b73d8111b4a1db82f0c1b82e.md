# 🎮 Katlans Game Module — Complete Game Engine Reference

> Pygame-style game development, fully built into Katlans.
> 

---

## Window & Game Loop

```
gm.init() ::
win = gmwin <"My Game", 800, 600> ::
gmwin.fps <60> ::
gmwin.icon <"icon.png"> ::
gmwin.full <true> ::
gmwin.resizable <true> ::
gmwin.bg <win, "#000000"> ::
gmwin.caption <"Score: 0"> ::
gmwin.clear <win> ::
gmwin.flip <win> ::
gmwin.update <win> ::
gm.quit() ::

~ Game Loop
clock = gmclock() ::

denta gm.running() |>
    deta event belongs in gm.events() |>
        if event.type == "quit" |>
            gm.stop() ::
        <|::
        if event.type == "keydown" |>
            if event.key == "escape" |>
                gm.stop() ::
            <|::
        <|::
    <|::

    fxn update <dt> ::
    gmwin.clear <win> ::
    fxn draw() ::
    gmwin.flip <win> ::
    dt = gmclock.tick <clock, 60> ::
<|::
```

---

## Surfaces & Blitting

```
surf = gmsurf <400, 300> ::
surf = gmsurf <400, 300, flags<"srcalpha">> ::  ~ transparent
surf = gmsurf.load <"sprite.png"> ::
surf = gmsurf.load.alpha <"sprite.png"> ::
gmsurf.blit <win, surf, x<100>, y<200>> ::
gmsurf.blit <win, surf, x<100>, y<200>, area<0,0,32,32>> ::
gmsurf.fill <surf, "#ff0000"> ::
gmsurf.copy <surf> ::
int w, int h = gmsurf.size <surf> ::
gmsurf.flip <surf, "h"> ::
gmsurf.rotate <surf, 45> ::
gmsurf.scale <surf, 2.0> ::
gmsurf.scale <surf, w<64>, h<64>> ::
gmsurf.alpha <surf, 128> ::
gmsurf.colorkey <surf, "#ff00ff"> ::         ~ transparent color
gmsurf.convert <surf> ::
gmsurf.convert.alpha <surf> ::
gmsurf.lock <surf> ::
gmsurf.unlock <surf> ::
x = gmsurf.pixel <surf, x, y> ::
gmsurf.pixel.set <surf, x, y, "#ff0000"> ::
```

---

## Drawing

```
gmdraw.rect <win, "#ff0000", 10, 10, 100, 50> ::
gmdraw.rect <win, "#ff0000", 10, 10, 100, 50, width<2>> ::  ~ outline
gmdraw.rect.rounded <win, "#ff0000", 10, 10, 100, 50, r<10>> ::
gmdraw.circle <win, "#00ff00", cx<100>, cy<100>, r<50>> ::
gmdraw.circle <win, "#00ff00", 100, 100, 50, width<2>> ::
gmdraw.ellipse <win, "#0000ff", 100, 100, 80, 50> ::
gmdraw.line <win, "#ffffff", 0, 0, 100, 100, width<2>> ::
gmdraw.lines <win, "#ffffff", pts, closed<false>, width<1>> ::
gmdraw.polygon <win, "#ffff00", pts> ::
gmdraw.arc <win, "#ff00ff", rect, start<0>, stop<3.14>, width<2>> ::
gmdraw.point <win, "#ffffff", 50, 50> ::
gmdraw.aaline <win, "#fff", 0, 0, 100, 100> ::   ~ anti-aliased
gmdraw.aalines <win, "#fff", pts, closed<false>> ::
```

---

## Sprites

```
~ Create sprite
sp = gmsprite <"sprite.png"> ::
sp = gmsprite <"sprite.png", x<100>, y<200>> ::
sp = gmsprite.sheet <"sheet.png", w<32>, h<32>, count<8>> ::
sp = gmsprite.animated <"sheet.png", w<32>, h<32>, fps<12>> ::

~ Properties
gmsp.pos <sp, 100, 200> ::
gmsp.vel <sp, vx<2.0>, vy<0.0>> ::
gmsp.acc <sp, ax<0.0>, ay<0.5>> ::
gmsp.scale <sp, 2.0> ::
gmsp.rotate <sp, 45> ::
gmsp.alpha <sp, 128> ::
gmsp.flip <sp, "h"> ::
gmsp.visible <sp, true> ::
gmsp.layer <sp, 2> ::
gmsp.img <sp, "newsprite.png"> ::

~ Actions
gmsp.update <sp, dt> ::
gmsp.move <sp, dx, dy> ::
gmsp.moveto <sp, x, y> ::
gmsp.draw <sp, win> ::
gmsp.kill <sp> ::

~ Animation
gmsp.anim.play <sp, "run"> ::
gmsp.anim.stop <sp> ::
gmsp.anim.pause <sp> ::
gmsp.anim.add <sp, "run", frames, fps<12>, loop<true>> ::
gmsp.anim.speed <sp, 1.5> ::

~ Groups
grp = gmgroup() ::
gmgroup.add <grp, sp> ::
gmgroup.add <grp, sp1, sp2, sp3> ::
gmgroup.remove <grp, sp> ::
gmgroup.clear <grp> ::
gmgroup.update <grp, dt> ::
gmgroup.draw <grp, win> ::
int n = gmgroup.count <grp> ::
```

---

## Input

```
~ Keyboard
true/false = gminput.key.pressed <"space"> ::
true/false = gminput.key.held <"a"> ::
true/false = gminput.key.released <"escape"> ::
lt keys = gminput.keys.held() ::
gminput.key.setrepeat <delay<300>, interval<100>> ::

~ Mouse
int x, int y = gminput.mouse.pos() ::
int dx, int dy = gminput.mouse.rel() ::
true/false = gminput.mouse.pressed <1> ::     ~ 1=left 2=mid 3=right
true/false = gminput.mouse.held <1> ::
gminput.mouse.visible <false> ::
gminput.mouse.pos.set <400, 300> ::

~ Joystick
lt joys = gminput.joy.list() ::
joy = gminput.joy.get <0> ::
flt x = gminput.joy.axis <joy, 0> ::
true/false = gminput.joy.btn <joy, 0> ::
gminput.joy.rumble <joy, 0.5, 0.5, 500> ::
```

---

## Events

```
deta event belongs in gm.events() |>
    if event.type == "quit" |> ... <|::
    if event.type == "keydown" |>
        display event.key ::             ~ "a"/"space"/"escape"
        display event.mod ::             ~ "ctrl"/"shift"
    <|::
    if event.type == "mousedown" |>
        display event.button / event.pos ::
    <|::
    if event.type == "mousemotion" |>
        display event.pos / event.rel ::
    <|::
    if event.type == "scroll" |>
        display event.x / event.y ::
    <|::
    if event.type == "resize" |>
        display event.w / event.h ::
    <|::
    if event.type == "custom" |>
        display event.data ::
    <|::
<|::

gm.event.post <"custom", data<{score, 100}>> ::
```

---

## Sound & Music

```
gmaudio.init <freq<44100>, size<-16>, channels<2>, buffer<512>> ::

~ Sound effects
snd = gmsound <"shoot.wav"> ::
gmsound.play <snd> ::
gmsound.play <snd, loops<-1>> ::
gmsound.stop <snd> ::
gmsound.fadeout <snd, ms<500>> ::
gmsound.vol <snd, 0.5> ::

~ Channels
ch = gmchan <0> ::
gmchan.play <ch, snd> ::
gmchan.stop <ch> ::
gmchan.pause <ch> / gmchan.unpause <ch> ::
gmchan.vol <ch, 0.8> ::
true/false = gmchan.busy <ch> ::
gmchan.queue <ch, snd> ::
gmchan.on.end <ch, fxn handle> ::

~ Music (streaming)
gmmusic.load <"bgm.mp3"> ::
gmmusic.play <loops<-1>> ::
gmmusic.stop() / gmmusic.pause() / gmmusic.unpause() ::
gmmusic.fadeout <ms<2000>> ::
gmmusic.vol <0.7> ::
gmmusic.pos <30.5> ::
true/false = gmmusic.playing() ::
gmmusic.on.end <fxn handle> ::

~ Generate
snd = gmaudio.gen.beep <freq<440>, ms<500>> ::
snd = gmaudio.gen.noise <ms<1000>> ::
```

---

## Text & Fonts

```
font = gmfont <"Arial", 24> ::
font = gmfont.load <"myfont.ttf", 24> ::
font = gmfont.sys <"Arial", 24, bold<true>, italic<false>> ::

surf = gmfont.render <font, "Hello World", "#ffffff", bg<"#000000">> ::
surf = gmfont.render <font, "Hello", "#fff", antialias<true>> ::

int w, int h = gmfont.size <font, "Hello"> ::
lt families = gmfont.list() ::

gmtext.draw <win, "Long text...", font, "#fff", rect<10,10,200,400>> ::
```

---

## Collision Detection

```
true/false = gmcol.rect <sp1, sp2> ::
true/false = gmcol.rect.point <sp, x, y> ::
true/false = gmcol.circle <sp1, sp2> ::
true/false = gmcol.mask <sp1, sp2> ::         ~ pixel-perfect
lt hits = gmcol.group <sp, grp> ::
lt hits = gmcol.groupgroup <grp1, grp2> ::
flt dist = gmcol.dist <sp1, sp2> ::
true/false = gmcol.line.rect <x1,y1,x2,y2, rect> ::
```

---

## Camera / Viewport

```
cam = gmcam <world_w<2000>, world_h<2000>> ::
gmcam.follow <cam, sp> ::
gmcam.pos <cam, x, y> ::
gmcam.zoom <cam, 1.5> ::
gmcam.shake <cam, intensity<5>, duration<500>> ::
gmcam.apply <cam, win, grp> ::
int wx, int wy = gmcam.world_to_screen <cam, x, y> ::
```

---

## Tilemap

```
tmap = gmtile.load <"map.tmx"> ::
tmap = gmtile.create <w<20>, h<15>, tile_w<32>, tile_h<32>> ::
gmtile.set <tmap, row, col, tile_id> ::
gmtile.draw <tmap, win, cam> ::
lt tiles = gmtile.layer <tmap, "ground"> ::
lt objs = gmtile.objects <tmap, "enemies"> ::
gmtile.tileset <tmap, "tileset.png", tw<32>, th<32>> ::
```

---

## Physics

```
world = gmphys.world <gravity<0, 9.8>> ::
body = gmphys.body <world, type<"dynamic">> ::
gmphys.body.pos <body, 100, 200> ::
gmphys.body.vel <body, 0, 0> ::
gmphys.body.mass <body, 1.0> ::
gmphys.body.friction <body, 0.3> ::
gmphys.body.restitution <body, 0.5> ::
gmphys.shape.rect <body, w<32>, h<32>> ::
gmphys.shape.circle <body, r<16>> ::
gmphys.step <world, dt> ::
gmphys.body.force <body, fx, fy> ::
gmphys.body.impulse <body, ix, iy> ::
gmphys.body.on.collision <body, fxn handle> ::
```

---

## Particles

```
pe = gmparticle.emitter <x<100>, y<100>> ::
gmparticle.rate <pe, 50> ::
gmparticle.life <pe, min<0.5>, max<2.0>> ::
gmparticle.speed <pe, min<50>, max<200>> ::
gmparticle.angle <pe, min<0>, max<360>> ::
gmparticle.size <pe, min<2>, max<8>> ::
gmparticle.color <pe, "#ff4400"> ::
gmparticle.fade <pe, true> ::
gmparticle.gravity <pe, 0, 100> ::
gmparticle.burst <pe, count<100>> ::
gmparticle.update <pe, dt> ::
gmparticle.draw <pe, win> ::
gmparticle.stop <pe> ::
```

---

## Save State

```
gmstate.save <"save.json", gameState> ::
ad state = gmstate.load <"save.json"> ::
gmstate.exists <"save.json"> ::
```