# 📷 Katlans VIS Module — Complete Computer Vision Reference

> Camera, face, hands, body, objects, OCR, motion — all built-in.
> 

---

## Camera Setup

```
cam = viscam <0> ::                          ~ 0 = default camera
cam = viscam <"rtsp://192.168.1.1/stream"> :: ~ IP camera
viscam.res <cam, 1920, 1080> ::
viscam.fps <cam, 60> ::
viscam.brightness <cam, 0.5> ::
viscam.contrast <cam, 0.5> ::
viscam.saturation <cam, 0.5> ::
viscam.hue <cam, 0.0> ::
viscam.autofocus <cam, true> ::
viscam.zoom <cam, 1.5> ::
viscam.exposure <cam, -5> ::
viscam.backend <cam, "opencv"> ::           ~ opencv/directshow/v4l2
int w = viscam.width <cam> ::
int h = viscam.height <cam> ::
true/false = viscam.open <cam> ::
viscam.close <cam> ::
lt cams = viscam.list() ::                   ~ list available cameras
```

---

## Frame Operations

```
frame = visframe <cam> ::
frame = visframe.blank <640, 480> ::
frame = visload <"image.jpg"> ::
vid = visvid <"video.mp4"> ::

visframe.show <frame, "Window"> ::
visframe.save <frame, "out.jpg"> ::
visframe.save <frame, "out.jpg", quality<95>> ::

~ Transforms
visframe.flip <frame, "h"> ::               ~ h/v/both
visframe.rotate <frame, 90> ::
visframe.resize <frame, 640, 480> ::
visframe.crop <frame, x, y, w, h> ::
visframe.pad <frame, 10, 10, 10, 10> ::
visframe.scale <frame, 0.5> ::
visframe.warp <frame, srcpts, dstpts> ::

~ Color conversion
visframe.gray <frame> ::
visframe.rgb <frame> ::
visframe.hsv <frame> ::
visframe.hls <frame> ::
visframe.lab <frame> ::
visframe.yuv <frame> ::

~ Adjustments
visframe.brightness <frame, 50> ::
visframe.contrast <frame, 1.5> ::
visframe.saturation <frame, 1.2> ::
visframe.gamma <frame, 1.2> ::

~ Filters
visframe.blur <frame, 5> ::
visframe.blur.median <frame, 5> ::
visframe.blur.bilateral <frame, 9, 75, 75> ::
visframe.sharpen <frame> ::
visframe.denoise <frame> ::

~ Effects
visframe.sepia <frame> ::
visframe.cartoon <frame> ::
visframe.sketch <frame> ::
visframe.pencil <frame> ::
visframe.oil <frame, 7, 1.0> ::
visframe.watercolor <frame> ::
visframe.pixelate <frame, 10> ::
visframe.vintage <frame> ::
visframe.vignette <frame, 0.5> ::

~ Thresholding
visframe.threshold <frame, 127, "binary"> ::
visframe.threshold.otsu <frame> ::
visframe.threshold.adaptive <frame, 11, 2> ::

~ Edge Detection
visframe.edges.canny <frame, 100, 200> ::
visframe.edges.sobel <frame> ::
visframe.edges.laplacian <frame> ::
visframe.edges.prewitt <frame> ::

~ Morphology
visframe.dilate <frame, 3> ::
visframe.erode <frame, 3> ::
visframe.open <frame, 3> ::
visframe.close <frame, 3> ::
visframe.gradient <frame, 3> ::

~ Contours
lt contours = visframe.contours <frame> ::
deta c belongs in contours |>
    display c.area / c.perimeter / c.bbox / c.center ::
    visdraw.contour <frame, c> ::
<|::

~ Histogram
hist = visframe.histogram <frame> ::
visframe.equalize <frame> ::
visframe.clahe <frame, 2.0> ::

~ Background
visframe.removebg <frame> ::
visframe.removebg.replace <frame, "bg.jpg"> ::
visframe.removebg.blur <frame, 15> ::
visframe.vbg <frame, "background.jpg"> ::
visframe.vbg.color <frame, "#00ff00"> ::

~ Overlay & Blend
visframe.overlay <frame, frame2, x, y, opacity<0.5>> ::
visframe.blend <frame1, frame2, 0.5> ::

~ Frame Info
int w, int h = visframe.size <frame> ::
int ch = visframe.channels <frame> ::
x = visframe.pixel <frame, x, y> ::
visframe.pixel.set <frame, x, y, r, g, b> ::
```

---

## Face Detection & Recognition

```
~ Detection
vis.facedetect <cam> |>
    fxn dec <onFace> <faces> |>
        deta f belongs in faces |>
            display f.x / f.y / f.w / f.h ::
            display f.conf / f.id ::
            visdraw.box <frame, f, color<"#00ff00">> ::
        <|::
    <|::
<|::

~ Face Mesh (468 landmarks)
vis.facemesh <cam> |>
    fxn dec <onMesh> <mesh> |>
        display mesh.landmarks ::
        display mesh.nose / mesh.leftEye / mesh.rightEye ::
        display mesh.lips / mesh.forehead / mesh.jaw ::
        visdraw.mesh <frame, mesh> ::
        visdraw.mesh.contours <frame, mesh> ::
        visdraw.mesh.tesselation <frame, mesh> ::
    <|::
<|::

~ Face Recognition
visface.add <"John", "john.jpg"> ::
visface.remove <"John"> ::
visface.load <"faces.db"> ::
visface.save <"faces.db"> ::

vis.facerecog <cam> |>
    fxn dec <onRecog> <results> |>
        deta r belongs in results |>
            display r.name / r.conf ::
            visdraw.box <frame, r> ::
            visdraw.label <frame, r.name, r.x, r.y> ::
        <|::
    <|::
<|::

~ Facial Landmarks (68 points)
vis.landmarks <cam> |>
    fxn dec <onLM> <lm> |>
        display lm.leftEyebrow / lm.rightEyebrow ::
        display lm.nose / lm.mouth / lm.jaw ::
        flt angle = lm.headangle ::
        flt pitch = lm.pitch ::
        flt yaw = lm.yaw ::
        flt roll = lm.roll ::
    <|::
<|::
```

---

## Hand Tracking

```
vis.hands <cam> maxhands<2> |>
    fxn dec <onHand> <hands> |>
        deta h belongs in hands |>
            display h.side / h.conf ::
            display h.landmarks ::
            display h.wrist ::
            display h.thumb.tip / h.index.tip ::
            display h.middle.tip / h.ring.tip / h.pinky.tip ::

            ~ Gestures
            true/false = h.fist ::
            true/false = h.open ::
            true/false = h.pointing ::
            true/false = h.peace ::
            true/false = h.thumbsup ::
            true/false = h.ok ::
            int fingers = h.fingersup ::
            x = h.gesture ::             ~ gesture name

            ~ Measurements
            flt dist = visdist <h.thumb.tip, h.index.tip> ::
            flt ang = visangle <h.wrist, h.index.mcp, h.index.tip> ::

            visdraw.hand <frame, h> ::
            visdraw.hand.connections <frame, h> ::
        <|::
    <|::
<|::
```

---

## Pose / Body Detection (33 Landmarks)

```
vis.pose <cam> complexity<1> smooth<true> |>
    fxn dec <onPose> <pose> |>
        display pose.landmarks ::
        display pose.nose ::
        display pose.leftShoulder / pose.rightShoulder ::
        display pose.leftElbow / pose.rightElbow ::
        display pose.leftWrist / pose.rightWrist ::
        display pose.leftHip / pose.rightHip ::
        display pose.leftKnee / pose.rightKnee ::
        display pose.leftAnkle / pose.rightAnkle ::
        display pose.leftHeel / pose.rightHeel ::
        display pose.leftToe / pose.rightToe ::

        flt angle = visangle <pose.leftShoulder, pose.leftElbow, pose.leftWrist> ::
        flt dist = visdist <pose.leftShoulder, pose.rightShoulder> ::
        x = pose.activity ::             ~ standing/sitting/walking etc.

        visdraw.pose <frame, pose> ::
        visdraw.pose.connections <frame, pose> ::
    <|::
<|::

~ Multi-person pose
vis.pose.multi <cam, 5> |>
    fxn dec <onPoses> <people> |>
        deta p belongs in people |>
            display p.id / p.pose ::
        <|::
    <|::
<|::
```

---

## Object Detection

```
vis.objects <cam> model<"yolo"> conf<0.5> |>
    fxn dec <onObj> <objects> |>
        deta o belongs in objects |>
            display o.label / o.conf ::
            display o.x / o.y / o.w / o.h ::
            visdraw.box <frame, o> ::
            visdraw.label <frame, o.label, o.x, o.y> ::
        <|::
    <|::
<|::

vis.objects.custom <cam, "mymodel.weights", "mymodel.cfg"> |>
    ~ custom trained model
<|::
```

---

## Segmentation

```
vis.segment <cam> |>
    fxn dec <onSeg> <seg> |>
        display seg.mask / seg.classes / seg.colored ::
        visdraw.segment <frame, seg> ::
    <|::
<|::

vis.segment.person <cam> |>
    fxn dec <onPerson> <mask> |>
        display mask.frame ::
    <|::
<|::
```

---

## Eye Tracking

```
vis.eyes <cam> |>
    fxn dec <onEye> <eyes> |>
        display eyes.left.x / eyes.left.y ::
        display eyes.right.x / eyes.right.y ::
        display eyes.gaze ::             ~ left/right/center/up/down
        display eyes.gazex / eyes.gazey ::
        true/false = eyes.blink ::
        true/false = eyes.leftBlink / eyes.rightBlink ::
        flt ear = eyes.openness ::
        true/false = eyes.drowsy ::
        visdraw.eyes <frame, eyes> ::
    <|::
<|::
```

---

## Emotion, Age & Gender

```
vis.emotion <cam> |>
    fxn dec <onEmo> <emo> |>
        display emo.dominant ::
        display emo.happy / emo.sad / emo.angry ::
        display emo.surprised / emo.neutral / emo.fearful / emo.disgusted ::
    <|::
<|::

vis.agegender <cam> |>
    fxn dec <onAG> <result> |>
        display result.age / result.age.range ::
        display result.gender / result.conf ::
    <|::
<|::
```

---

## Motion & Optical Flow

```
vis.motion <cam> sensitivity<500> |>
    fxn dec <onMotion> <motion> |>
        true/false = motion.detected ::
        display motion.regions / motion.area ::
        visdraw.box <frame, motion.regions> ::
    <|::
<|::

vis.flow <cam> |>
    fxn dec <onFlow> <flow> |>
        display flow.vectors / flow.magnitude / flow.angle ::
        visdraw.flow <frame, flow> ::
    <|::
<|::
```

---

## Color Detection

```
vis.color <cam, "#ff0000"> |>
    fxn dec <onColor> <result> |>
        display result.mask / result.regions / result.area ::
    <|::
<|::

lt colors = viscol.dominant <frame, 5> ::
display colors[0].hex / colors[0].rgb / colors[0].percent ::
```

---

## QR, Barcode, OCR & License Plate

```
vis.qr <cam> |>
    fxn dec <onQR> <codes> |>
        deta c belongs in codes |>
            display c.data / c.type / c.x / c.y ::
        <|::
    <|::
<|::

~ OCR
x = visocr <frame> ::
display x.text / x.words / x.lines / x.conf / x.boxes ::
x = visocr <frame, lang<"eng+hin">> ::

~ License Plate
vis.plate <cam> |>
    fxn dec <onPlate> <plates> |>
        deta p belongs in plates |>
            display p.text / p.conf / p.x / p.y ::
        <|::
    <|::
<|::
```

---

## Feature Matching

```
lt matches = vistemplate <frame, "template.png", threshold<0.8>> ::
kp, desc = visfeature.detect <frame, "orb"> ::
lt matches = visfeature.match <frame1, frame2, "orb"> ::
visdraw.keypoints <frame, kp> ::
visdraw.matches <frame1, frame2, matches> ::
H = vishomography <srcpts, dstpts> ::
```

---

## Drawing Tools

```
visdraw.box <frame, x, y, w, h, color<"#f00">, thickness<2>> ::
visdraw.circle <frame, cx, cy, r, color<"#0f0">, filled<true>> ::
visdraw.line <frame, x1,y1, x2,y2, color<"#00f">, thickness<2>> ::
visdraw.arrow <frame, x1,y1, x2,y2, color<"#f00">> ::
visdraw.polygon <frame, pts, color<"#ff0">, filled<true>> ::
visdraw.text <frame, x, y, "Hello", size<1.0>, color<"#fff">> ::
visdraw.point <frame, x, y, color<"#f00">, size<5>> ::
visdraw.grid <frame, step<50>, color<"#ccc">> ::
visdraw.cross <frame, x, y, size<20>, color<"#f00">> ::
visdraw.heatmap <frame, data> ::
visdraw.skeleton <frame, landmarks> ::
visdraw.ruler <frame, x1,y1, x2,y2> ::
```

---

## Pipeline & Recording

```
vis.pipeline <cam, "face", "hands", "pose", "objects"> |>
    fxn dec <onResult> <result> |>
        display result.face / result.hands / result.pose / result.objects ::
        visframe.show <result.frame, "Pipeline"> ::
    <|::
<|::

visrec.start <"output.mp4", fps<30>, codec<"mp4v">> ::
visrec.frame <frame> ::
visrec.stop ::
visshot <"screenshot.png"> ::
```

---

## Video File Processing

```
vid = visvid <"video.mp4"> ::
int fps = visvid.fps <vid> ::
int frames = visvid.framecount <vid> ::
flt dur = visvid.duration <vid> ::
visvid.seek <vid, 30.5> ::
frame = visvid.read <vid> ::
visvid.close <vid> ::

writer = visvid.writer <"out.mp4", fps<30>, size<640,480>> ::
visvid.write <writer, frame> ::
visvid.close.writer <writer> ::
```