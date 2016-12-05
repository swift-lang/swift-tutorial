#!/bin/bash

# This script seems to be customized for user 'yadunandb', intentional?

source setup.sh

export BEAGLE_USERNAME="yadunandb"
export BEAGLE_PROJECT="CI-CCR000013"
export MIDWAY_USERNAME="yadunand"
export AWS_CREDENTIALS_FILE="/home/yadu/.ssh/swift-grant-credentials.csv"
export URL_OF_AD_HOC_MACHINE_1="crank"
export AD_HOC_1_USERNAME="yadunand"
export OSG_USERNAME="yadunand"
export OSG_PROJECT="swift"
export GORDON_USERNAME="yadunand"
export TRESTLES_USERNAME="yadunand"
export HOPPER_USERNAME="yadunand"
export BLACKLIGHT_USERNAME="yadunand"


if 0 ;then
for i in $(seq 1 1 3)
do
    pushd .
    echo "============================TESTING part0$i==========================="
    cd part0$i
    swift p$i.swift -site=localhost
    if [[ $? == 0 ]]
    then
        echo "Cleaning up!"
        cleanup
    fi
    echo -e "\n\n"
    popd
done
fi

#SITES=('beagle' 'midway' 'osgc' 'ad-hoc-1')
#SITES=('stampede' 'gordon' 'blacklight')
SITES=('edison')
CONFIG="-config nersc.conf"
for i in $(seq 4 1 6)
do
    pushd .
    echo "============================TESTING part0$i==========================="
    cd part0$i

    for SITE in ${SITES[*]}
    do
        echo "Running on SITE : $SITE"
        swift $CONFIG -sites $SITE p$i.swift
        if [[ $? == 0 ]]
        then
            echo "Cleaning up!"
            cleanup
        fi
    done
    echo -e "\n\n"
    popd
done


SITES=('edison_multinode')
CONFIG="-config nersc.conf"
for i in $(seq 7 1 8)
do
    pushd .
    echo "============================TESTING part0$i==========================="
    cd part0$i

    for SITE in ${SITES[*]}
    do
        echo "Running on SITE : $SITE"
        swift $CONFIG -sites $SITE p$i.swift
        if [[ $? == 0 ]]
        then
            echo "Cleaning up!"
            cleanup
        fi
    done
    echo -e "\n\n"
    popd
done
