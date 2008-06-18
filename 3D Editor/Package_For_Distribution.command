#!/bin/bash
#automatic packaging script by Niemand. 5/10/2008

#relevant paths
#product is expected to be an application bundle 
#	in $BUILD_DIR/$PRODUCT_DIR
BUILD_DIR="build"
PRODUCT_DIR="Deployment"
#created archives will be written to
#	$BUILD_DIR/$OUPUT_DIR
OUPUT_DIR="Current"

#move to the directory the command file was run from
cd "$(dirname "$0")"
#check if expected directory structure exists
if [ ! -d $BUILD_DIR ]
then
	echo "'./$BUILD_DIR' does not exist, aborting"
	exit
fi
if [ ! -d "$BUILD_DIR/$PRODUCT_DIR" ]
then
	echo "'./$BUILD_DIR/$PRODUCT_DIR' does not exist, aborting"
	exit
fi
#move to the built product
cd ./$BUILD_DIR/$PRODUCT_DIR/
#get the product name
if [ `ls *.app | wc -l` = 0 ]
then
	echo "No built product found, aborting"
	exit
fi
APPNAME=`ls | sed -n 's/\([^.]*\).app/\1/p'`
echo "Product name is: $APPNAME"
#create the output directory if necessary
cd ..
if [ ! -d "$OUPUT_DIR" ] 
then 
	mkdir "$OUPUT_DIR"
	echo "Created './$BUILD_DIR/$OUPUT_DIR' to contain output."
fi
#move to the output directory
cd $OUPUT_DIR/
#erase old output if present
if [ -f "$APPNAME.tgz" ] 
then 
	rm "$APPNAME.tgz"
	echo "Erased old $APPNAME.tgz"
fi
if [ -f "$APPNAME Source Code.tgz" ] 
then 
	rm "$APPNAME Source Code.tgz"
	echo "Erased old $APPNAME Source Code.tgz"
fi
#back to the product directory
cd ../$PRODUCT_DIR/
#make a compressed copy of the product
echo "Compressing product..."
tar -czf "../$OUPUT_DIR/$APPNAME.tgz" "./$APPNAME.app"
#make a copy of the source code
#move mack up to the original directory
cd ../..
#make a clean copy of the project
echo "Exporting source code..."
svn export ./ "./$BUILD_DIR/$OUPUT_DIR/$APPNAME Source Code"
#test to see whether we got anything
if [ ! -d "./$BUILD_DIR/$OUPUT_DIR/$APPNAME Source Code" ]
then
	echo "export failed, aborting"
	exit
fi
#go to the output directory
cd ./$BUILD_DIR/$OUPUT_DIR/
#make a compressed copy of the copy
echo "Compressing source code..."
tar -czf "./$APPNAME Source Code.tgz" "./$APPNAME Source Code"
#delete the uncompressed copy
echo "Deleting temporary source code copy..."
rm -r "./$APPNAME Source Code"
echo "Finished"