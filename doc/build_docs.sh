#!/bin/bash -e

asciidoc -a icons -a toc -a toplevels=3 -a stylesheet=$PWD/asciidoc.css -a max-width=800px -o tutorial.html README
asciidoc -a icons -a stylesheet=$PWD/asciidoc.css -a max-width=800px -o amazon_ec2.html amazon_ec2.asc
asciidoc -a icons -a stylesheet=$PWD/asciidoc.css -a max-width=800px -o swift-config.html swift.conf.asc


