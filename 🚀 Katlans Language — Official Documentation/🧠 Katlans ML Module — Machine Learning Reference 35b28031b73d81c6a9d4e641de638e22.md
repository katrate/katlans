# 🧠 Katlans ML Module — Machine Learning Reference

> PyTorch + TensorFlow style ML, fully built into Katlans.
> 

---

## Tensors

```
~ Create
t = mltensor <1, 2, 3, 4, 5> ::
t = mltensor <[[1,2],[3,4]]> ::
t = mlzeros <3, 4> ::
t = mlones <3, 4> ::
t = mlfull <3, 4, 7.0> ::
t = mleye <4> ::                            ~ identity matrix
t = mlrand <3, 4> ::
t = mlrandn <3, 4> ::
t = mlarange <0, 10, 0.5> ::
t = mllinspace <0, 1, 100> ::
t = mltensor <1, 2, 3> dtype<"float32"> ::

~ Shape
lt shape = mlt.shape <t> ::
t2 = mlt.reshape <t, 2, 3> ::
t2 = mlt.flatten <t> ::
t2 = mlt.squeeze <t> ::
t2 = mlt.unsqueeze <t, 0> ::
t2 = mlt.transpose <t, 0, 1> ::
t2 = mlt.permute <t, 2, 0, 1> ::
t2 = mlt.view <t, -1, 3> ::

~ Indexing
x = t[0] ::
x = t[0, 1] ::
x = t[0:5] ::
x = t[:, 1] ::
x = t[t > 0] ::                            ~ boolean mask

~ Math
t2 = mlt.add <t1, t2> ::
t2 = mlt.sub <t1, t2> ::
t2 = mlt.mul <t1, t2> ::
t2 = mlt.div <t1, t2> ::
t2 = mlt.pow <t, 2> ::
t2 = mlt.sqrt <t> ::
t2 = mlt.exp <t> ::
t2 = mlt.log <t> ::
t2 = mlt.sigmoid <t> ::
t2 = mlt.relu <t> ::
t2 = mlt.clamp <t, 0.0, 1.0> ::

~ Reductions
x = mlt.sum <t> ::
x = mlt.mean <t> ::
x = mlt.std <t> ::
x = mlt.max <t> ::
x = mlt.min <t> ::
x = mlt.argmax <t> ::
x = mlt.norm <t> ::

~ Linear Algebra
t2 = mlt.matmul <t1, t2> ::
t2 = mlt.dot <t1, t2> ::
t2 = mlt.inv <t> ::
t2 = mlt.det <t> ::
u, s, v = mlt.svd <t> ::
vals, vecs = mlt.eig <t> ::

~ Concat
t2 = mlt.cat <t1, t2, dim<0>> ::
t2 = mlt.stack <t1, t2, dim<0>> ::
lt parts = mlt.split <t, 3, dim<0>> ::

~ Device
t = mlt.gpu <t> ::
t = mlt.cpu <t> ::
true/false = ml.gpu.available() ::

~ Gradients
t = mltensor <1.0, 2.0> grad<true> ::
t.backward() ::
x = t.grad ::
```

---

## Neural Network Layers

```
~ Sequential model
model = mlmodel.seq |>
    mllayer.linear <128, 64> ::
    mllayer.relu() ::
    mllayer.dropout <0.2> ::
    mllayer.linear <64, 10> ::
    mllayer.softmax <dim<1>> ::
<|::

~ Linear / Dense
mllayer.linear <in<128>, out<64>, bias<true>> ::

~ Convolutions
mllayer.conv1d <in<1>, out<32>, kernel<3>, stride<1>, pad<1>> ::
mllayer.conv2d <in<3>, out<64>, kernel<3>, stride<1>, pad<1>> ::
mllayer.conv3d <in<1>, out<32>, kernel<3>> ::
mllayer.convtranspose2d <in<64>, out<32>, kernel<4>, stride<2>, pad<1>> ::

~ Pooling
mllayer.maxpool2d <kernel<2>, stride<2>> ::
mllayer.avgpool2d <kernel<2>, stride<2>> ::
mllayer.adaptiveavgpool2d <output<1,1>> ::
mllayer.globalavgpool() ::

~ Normalization
mllayer.batchnorm1d <128> ::
mllayer.batchnorm2d <64> ::
mllayer.layernorm <128> ::
mllayer.groupnorm <groups<8>, channels<64>> ::

~ Recurrent
mllayer.rnn <input<10>, hidden<64>, layers<2>, bidir<true>> ::
mllayer.lstm <input<10>, hidden<64>, layers<2>> ::
mllayer.gru <input<10>, hidden<64>, layers<2>> ::

~ Attention & Transformer
mllayer.multiheadattn <embed<512>, heads<8>> ::
mllayer.transformer <d_model<512>, nhead<8>, layers<6>> ::
mllayer.transformerencoder <d_model<512>, nhead<8>, layers<6>> ::

~ Embedding
mllayer.embedding <vocab<10000>, dim<256>> ::
mllayer.posembedding <maxlen<512>, dim<256>> ::

~ Activations
mllayer.relu() ::
mllayer.leakyrelu <0.01> ::
mllayer.gelu() ::
mllayer.sigmoid() ::
mllayer.tanh() ::
mllayer.softmax <dim<1>> ::
mllayer.mish() ::
mllayer.swish() ::

~ Regularization
mllayer.dropout <0.2> ::
mllayer.dropout2d <0.2> ::

~ Shape
mllayer.flatten() ::
mllayer.reshape <-1, 3, 224, 224> ::
mllayer.permute <0, 2, 1> ::
```

---

## Loss Functions

```
loss = mlloss.mse() ::
loss = mlloss.mae() ::
loss = mlloss.crossentropy() ::
loss = mlloss.bce() ::
loss = mlloss.bcewithlogits() ::
loss = mlloss.nll() ::
loss = mlloss.huber <delta<1.0>> ::
loss = mlloss.hinge() ::
loss = mlloss.kldiv() ::
loss = mlloss.cosine() ::
loss = mlloss.ctc() ::
loss = mlloss.focal <gamma<2.0>> ::
loss = mlloss.dice() ::
loss = mlloss.iou() ::

val = mlloss.compute <loss, pred, target> ::
```

---

## Optimizers

```
opt = mlopt.sgd <model, lr<0.01>, momentum<0.9>, weight_decay<1e-4>> ::
opt = mlopt.adam <model, lr<0.001>, betas<0.9, 0.999>> ::
opt = mlopt.adamw <model, lr<0.001>, weight_decay<0.01>> ::
opt = mlopt.rmsprop <model, lr<0.01>> ::
opt = mlopt.adagrad <model, lr<0.01>> ::
opt = mlopt.nadam <model, lr<0.002>> ::

mlopt.step <opt> ::
mlopt.zero_grad <opt> ::

~ LR Schedulers
sched = mlsched.step <opt, step_size<10>, gamma<0.1>> ::
sched = mlsched.cosine <opt, T_max<100>> ::
sched = mlsched.plateau <opt, patience<5>, factor<0.5>> ::
sched = mlsched.warmup <opt, warmup_steps<1000>> ::
sched = mlsched.cyclic <opt, base_lr<0.001>, max_lr<0.01>> ::
mlsched.step <sched> ::
```

---

## Training Loop

```
deta epoch belongs range(1;100;1) |>
    model = mlmodel.train <model> ::
    deta batch belongs in dataloader |>
        inputs, labels = batch ::
        mlopt.zero_grad <opt> ::
        outputs = mlmodel.forward <model, inputs> ::
        val = mlloss.compute <loss, outputs, labels> ::
        mlback.backward <val> ::
        mlback.clip_grad <model, max_norm<1.0>> ::
        mlopt.step <opt> ::
    <|::
    mlsched.step <sched> ::
<|::

~ Validation
model = mlmodel.eval <model> ::
mlt.grad.disable() ::
deta batch belongs in val_loader |>
    outputs = mlmodel.forward <model, inputs> ::
<|::
mlt.grad.enable() ::

~ Inference
output = mlmodel.predict <model, input> ::
lt probs = mlmodel.predict.proba <model, input> ::
int cls = mlmodel.predict.class <model, input> ::
```

---

## Dataset & DataLoader

```
ds = mlds.create <"./data"> ::
mlds.transform <ds, "resize", 224, 224> ::
mlds.transform <ds, "normalize", mean<0.5,0.5,0.5>, std<0.5,0.5,0.5>> ::
mlds.transform <ds, "totensor"> ::
mlds.transform <ds, "randomflip"> ::
mlds.transform <ds, "randomcrop", 224> ::
mlds.transform <ds, "colorjitter", brightness<0.2>> ::
mlds.transform <ds, "rotate", degrees<30>> ::

ds_train, ds_val, ds_test = mlds.split <ds, 0.7, 0.15, 0.15> ::

dl = mldl.create <ds_train, batch<32>, shuffle<true>, workers<4>> ::

~ Built-in datasets
ds = mlds.mnist <"./data", train<true>, download<true>> ::
ds = mlds.cifar10 <"./data", train<true>> ::
ds = mlds.imagenet <"./data", split<"train">> ::
ds = mlds.csv <"data.csv", target<"label">> ::
ds = mlds.image_folder <"./images"> ::
ds = mlds.text <"data.txt", vocab_size<10000>, seq_len<128>> ::
```

---

## Save, Load & Export

```
mlmodel.save <model, "model.kml"> ::
mlmodel.save.weights <model, "weights.kml"> ::
mlmodel.save.checkpoint <model, opt, epoch, loss, "checkpoint.kml"> ::

model = mlmodel.load <"model.kml"> ::
mlmodel.load.weights <model, "weights.kml"> ::
model, opt, epoch, loss = mlmodel.load.checkpoint <"checkpoint.kml"> ::

mlmodel.export.onnx <model, "model.onnx", input_sample> ::
mlmodel.export.torchscript <model, "model.pt"> ::
```

---

## Pretrained Models

```
model = mlpretrained.resnet <50, pretrained<true>> ::
model = mlpretrained.vgg <16, pretrained<true>> ::
model = mlpretrained.efficientnet <"b0", pretrained<true>> ::
model = mlpretrained.mobilenet <"v3", pretrained<true>> ::
model = mlpretrained.bert <pretrained<true>> ::
model = mlpretrained.gpt2 <pretrained<true>> ::
model = mlpretrained.t5 <"base", pretrained<true>> ::
model = mlpretrained.whisper <"small", pretrained<true>> ::
model = mlpretrained.clip <pretrained<true>> ::
model = mlpretrained.yolo <"v8", pretrained<true>> ::

~ Fine-tune
mlmodel.freeze <model> ::
mlmodel.unfreeze <model> ::
mlmodel.freeze.layers <model, "conv1", "conv2"> ::
mlmodel.replace_head <model, 10> ::          ~ replace classifier
```

---

## Metrics

```
x = mlmetric.accuracy <pred, target> ::
x = mlmetric.topk_accuracy <pred, target, k<5>> ::
x = mlmetric.precision <pred, target> ::
x = mlmetric.recall <pred, target> ::
x = mlmetric.f1 <pred, target> ::
x = mlmetric.auc <pred, target> ::
x = mlmetric.mse <pred, target> ::
x = mlmetric.r2 <pred, target> ::
x = mlmetric.iou <boxes1, boxes2> ::
x = mlmetric.map <pred, target> ::
x = mlmetric.bleu <pred, target> ::
x = mlmetric.confusion_matrix <pred, target> ::
mlmetric.report <pred, target> ::
```

---

## Callbacks & Utilities

```
mlcallback.early_stop <patience<10>, monitor<"val_loss">> ::
mlcallback.checkpoint <"best.kml", monitor<"val_acc">, mode<"max">> ::
mlcallback.lr_finder <model, dl, opt> ::
mlcallback.tensorboard <"./logs"> ::
mlcallback.progress <epochs<100>> ::

~ Mixed precision
mlamp.enable() ::
mlamp.scale <loss> ::
```