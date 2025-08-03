# If bin directory doesn't exist, then create it.
if [ ! -d ../bin ]; then
    mkdir ../bin
    echo Created ../bin directory 
fi 

# If res directory doesn't exist, then create it.
if [ ! -d ../res ]; then
    mkdir ../res
    echo Created ../res directory 
fi 

# Unpack and build Randomizer
RandomizerVer=1.0
wget -O ../res/Randomizer_v$RandomizerVer.tar.gz https://github.com/krmnino/Randomizer/archive/refs/tags/v$RandomizerVer.tar.gz
tar -xvzf ../res/Randomizer_v$RandomizerVer.tar.gz --directory ../res
rm -r ../res/Randomizer_v1.0.tar.gz
cd ../res/Randomizer-$RandomizerVer/src
sh Initialize.sh
cd -
make -C ../res/Randomizer-$RandomizerVer/src
cp ../res/Randomizer-$RandomizerVer/src/Randomizer.hpp ../res
cp ../res/Randomizer-$RandomizerVer/res/libRandomizer.so ../res
rm -r ../res/Randomizer-$RandomizerVer
