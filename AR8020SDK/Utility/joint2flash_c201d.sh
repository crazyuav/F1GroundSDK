#!/bin/bash


function dec2hex()
{
    printf "%x" $1
}

outputapptxt=app.bin
outputtmp=apptmp.bin


# 一下配置为build SDK 的时候使用
Lib=../Application
gndcpu0=$Lib/cpu0/ar8020_gndcpu0.bin
gndcpu1=$Lib/cpu1/ar8020_gndcpu1.bin
gndcpu2=$Lib/cpu2/ar8020_gndcpu2.bin
outcfgbin=cfgdata.bin

echo "Making the image package, please wait ..."

#get the length of outcfgbin cpu0cpu1/gndcpu2.txt
gndcpu0length=`stat --format=%s $gndcpu0`
gndcpu1length=`stat --format=%s $gndcpu1`
gndcpu2length=`stat --format=%s $gndcpu2`
cfgbinlenggth=`stat --format=%s $outcfgbin`


echo "gndcpu0length = $gndcpu0length, gndcpu1length = $gndcpu1length, gndcpu2length = $gndcpu2length, cfgbinlenggth = $cfgbinlenggth"


if [ -f $outputtmp ]; then
    rm $outputtmp
fi

if [ -f $outputapptxt ]; then
    rm $outputapptxt
fi

imagedate=`date "+%Y%m%d%H%M%S"`

# 添加 imagedate 信息
echo -n -e \\x0 >> $outputtmp
for i in {0..6}
do
        shiftlen=$[ i * 2 + 1]
        tmp=`expr substr "$imagedate" $shiftlen 2`
        echo -n -e \\x$tmp >> $outputtmp
done


#add "0" to the 8byte for Reserve0
dd if=/dev/zero of=zero.image bs=8 count=1
cat zero.image >> $outputtmp

#add "0" to the 8byte for Reserve1
dd if=/dev/zero of=zero.image bs=8 count=1
cat zero.image >> $outputtmp

#add "0" to the 8byte for Reserve2
dd if=/dev/zero of=zero.image bs=8 count=1
cat zero.image >> $outputtmp

#store cpu0 length 4byte
for i in {0..3}
do
        shiftlen=$[ i * 8 ]
        tmp=`echo $gndcpu0length $shiftlen | awk '{print rshift($1,$2)}'`
        tmp=`echo $tmp | awk '{print and($1,255)}'`
        tmphex=$(dec2hex $tmp)
        echo -n -e \\x$tmphex >> $outputtmp
done

#store cpu1 length 4byte
for i in {0..3}
do
        shiftlen=$[ i * 8 ]
        tmp=`echo $gndcpu1length $shiftlen | awk '{print rshift($1,$2)}'`
        tmp=`echo $tmp | awk '{print and($1,255)}'`
        tmphex=$(dec2hex $tmp)
        echo -n -e \\x$tmphex >> $outputtmp
done

#store cpu2 length 4byte
for i in {0..3}
do
        shiftlen=$[ i * 8 ]
        tmp=`echo $gndcpu2length $shiftlen | awk '{print rshift($1,$2)}'`
        tmp=`echo $tmp | awk '{print and($1,255)}'`
        tmphex=$(dec2hex $tmp)
        echo -n -e \\x$tmphex >> $outputtmp
done

#store cfg.bin length 4byte
for i in {0..3}
do
        shiftlen=$[ i * 8 ]
        tmp=`echo $cfgbinlenggth $shiftlen | awk '{print rshift($1,$2)}'`
        tmp=`echo $tmp | awk '{print and($1,255)}'`
        tmphex=$(dec2hex $tmp)
        echo -n -e \\x$tmphex >> $outputtmp
done

#add "0" to the 4K offset
zerolengthboot=$((4096 - 48))
dd if=/dev/zero of=zero.image bs=$zerolengthboot count=1
cat zero.image >> $outputtmp

#add "0" to the 60K offset for the baseband config and foctory setting or user setting
dd if=/dev/zero of=zero.image bs=61440 count=1
cat zero.image >> $outputtmp

#add cpu0 image
cat $gndcpu0 >> $outputtmp

#add cpu1 image
cat $gndcpu1 >> $outputtmp

#add cpu2 image
cat $gndcpu2 >> $outputtmp

#add config bin
cat $outcfgbin >> $outputtmp 

cat $outputtmp >> $outputapptxt
chmod +x $outputapptxt

rm zero.image
rm $outputtmp

echo "cf_log: -----------bin is ok----------------------------"





