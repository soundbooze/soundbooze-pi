#!/bin/sh

## TODO: tube extract ##
## TODO: int(, paramSlider, functions ... ##

NAME=`echo "$1" | cut -d'.' -f1`

sed 's/FAUSTFLOAT/float/g' $1 > foo.c
sed 's/float(/(float)(/g' foo.c > bar.c
sed 's/fSamplingFreq/samplingFreq/g' bar.c > foo.c
sed 's/virtual void/inline void/g' foo.c > bar.c
sed 's/input0/input/g' bar.c > foo.c 
sed 's/output0/output/g' foo.c > bar.c
sed 's/int samplingFreq/uint32_t samplingFreq/g' bar.c > foo.c

INIT='s/inline void instanceConstants/inline void init_'$NAME'/g'
echo "sed '$INIT' foo.c > bar.c" > init-1.sh 
chmod 755 init-1.sh 
./init-1.sh

COMPUTE='s/inline void compute/inline void compute_'$NAME'/g'
echo "sed '$COMPUTE' bar.c > foo.c" > init-2.sh 
chmod 755 init-2.sh 
./init-2.sh 

sed 's/int samplingFreq/uint32_t samplingFreq/g' foo.c > bar.c

sed 's/float\*\* input/float \*input/g' bar.c > foo.c
sed 's/float\*\* output/float \*output/g' foo.c > $1

rm -f init-1.sh
rm -f init-2.sh
rm -f foo.c bar.c

cat $1 | egrep -v 'include|ifndef|define|endif|private|public|\/|};' > strip.c 

echo "
inline float 
min (float a, float b) {
  if (a > b)
    return b;
  return a;
}

inline float 
max (float a, float b) {
  if (a < b)
    return b;
  return a;
}
" > minmax.c

cat minmax.c strip.c > $1
