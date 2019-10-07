#!/bin/sh

makefile=Makefile.files
cppfile=cpp.list
ls -ort *.cpp|grep -v AI|awk '{print $8}' |awk -F.  '{print $1}' > $cppfile

cat /dev/null > $makefile

echo "" >> $makefile
echo "" >> $makefile
echo "" >> $makefile

echo "MODULE_LIBS =   \\" >> $makefile
cat $cppfile|while read line
do
        echo "          ./libs/$line.o \\" >> $makefile
        #echo $line
done


echo "" >> $makefile
echo "" >> $makefile
echo "" >> $makefile



echo "MODULE_SRC =     \\" >> $makefile
cat $cppfile|while read line
do
        echo "          ./$line.o \\" >> $makefile
        #echo $line
done



