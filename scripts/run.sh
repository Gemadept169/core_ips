#! /bin/sh
dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

export LD_LIBRARY_PATH=$dirname/lib
export SOT_DATA_PATH=$dirname/data/sot
cd $dirname/bin && ./core_ips