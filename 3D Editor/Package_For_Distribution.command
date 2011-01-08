#!/bin/bash
#automatic packaging script by Niemand. 5/10/2008

#relevant paths
#product is expected to be an application bundle 
#	in $BUILD_DIR/$PRODUCT_DIR
BUILD_DIR="build"
PRODUCT_DIR="Release"
#created archives will be written to
#	$BUILD_DIR/$OUPUT_DIR
OUPUT_DIR="Current"
#use this key to generate update signature
PRIVATE_KEY_PATH="/Volumes/Vault/Home/3D_BoA_su_dsa_priv.pem"

APPCAST_URL_BASE="http://svn.hallsofchaos.net/3D_BoA_Editor"

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
APPNAME=`ls -1 | sed -n 's/\([^.]*\).app$/\1/p'`
echo "Product name is: $APPNAME"
HUMAN_VERSION=`grep -m 1 -A 1 'CFBundleShortVersionString' "$APPNAME.app/Contents/Info.plist" | tail -n 1 | sed "s|[	]*<string>\([0-9\.]*\)</string>|\1|"`
SVN_REVISION=`grep -m 1 -A 1 'CFBundleVersion' "$APPNAME.app/Contents/Info.plist" | tail -n 1 | sed "s|[	]*<string>\([0-9\.]*\)</string>|\1|"`
echo "Version is $HUMAN_VERSION($SVN_REVISION)"

PRODUCT_ARCHIVE_NAME="3D Editor v$HUMAN_VERSION (Mac).tgz"
SOURCE_ARCHIVE_NAME="3D Editor Source v$HUMAN_VERSION (Mac).tgz"
APPCAST_ENTRY="v$HUMAN_VERSION.xml"
RELEASE_NOTES="rnotes_$HUMAN_VERSION.html"

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
if [ -f "$PRODUCT_ARCHIVE_NAME" ] 
then 
	rm "$PRODUCT_ARCHIVE_NAME"
	echo "Erased old $PRODUCT_ARCHIVE_NAME"
fi
if [ -f "$SOURCE_ARCHIVE_NAME" ] 
then 
	rm "$SOURCE_ARCHIVE_NAME"
	echo "Erased old $SOURCE_ARCHIVE_NAME"
fi
if [ -f "$APPCAST_ENTRY" ] 
then 
	rm "$APPCAST_ENTRY"
	echo "Erased old $APPCAST_ENTRY"
fi
#do _not_ erase release notes, since we only generate a skeleton, and someone may have filled it in

#back to the product directory
cd ../$PRODUCT_DIR/
#make a compressed copy of the product
echo "Compressing product..."
tar -czf "../$OUPUT_DIR/$PRODUCT_ARCHIVE_NAME" "./$APPNAME.app"
#make a copy of the source code
#move back up to the original directory
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
#make a compressed copy of the code
echo "Compressing source code..."
tar -czf "./$SOURCE_ARCHIVE_NAME" "./$APPNAME Source Code"
#delete the uncompressed copy
echo "Deleting temporary source code copy..."
rm -r "./$APPNAME Source Code"

#generate appcast entry
if [ -f "$PRIVATE_KEY_PATH" ]
then	
	UPDATE_SIZE=`stat -f %z "$PRODUCT_ARCHIVE_NAME"`
	PUBDATE=$(LC_TIME=en_US date +"%a, %d %b %G %T %z")
	SIGNATURE=`openssl dgst -sha1 -binary < "$PRODUCT_ARCHIVE_NAME" | openssl dgst -dss1 -sign "$PRIVATE_KEY_PATH" | openssl enc -base64`
	
	echo '			<item>' > $APPCAST_ENTRY
	echo "				<title>Version $HUMAN_VERSION</title>" >> $APPCAST_ENTRY
	echo "					<sparkle:releaseNotesLink>" >> $APPCAST_ENTRY
	echo "						$APPCAST_URL_BASE/ReleaseNotes/Mac/rnotes_$HUMAN_VERSION.html" >> $APPCAST_ENTRY
	echo "					</sparkle:releaseNotesLink>" >> $APPCAST_ENTRY
	echo "					<pubDate>$PUBDATE</pubDate>" >> $APPCAST_ENTRY
	echo "					<enclosure url=\"$APPCAST_URL_BASE/Versions/Mac/$PRODUCT_ARCHIVE_NAME\" sparkle:version=\"$SVN_REVISION\" sparkle:shortVersionString=\"$HUMAN_VERSION\" length=\"$UPDATE_SIZE\" type=\"application/octet-stream\" sparkle:dsaSignature=\"$SIGNATURE\" />" >> $APPCAST_ENTRY
	echo '			</item>' >> $APPCAST_ENTRY
	
	echo "Wrote appcast entry data to $APPCAST_ENTRY"
else
	echo "Unable to locate Sparkle private key at $PRIVATE_KEY_PATH, aborting appcast entry generation"
	exit 1
fi

if [ ! -f "$RELEASE_NOTES" ]
then
	#generate release notes skeleton
	echo '<html>' > $RELEASE_NOTES
	echo '	<head>' >> $RELEASE_NOTES
	echo '		<meta http-equiv="content-type" content="text/html;charset=utf-8">' >> $RELEASE_NOTES
	echo '		<title>3D Blades of Avernum Editor</title>' >> $RELEASE_NOTES
	echo '		<meta name="robots" content="anchors">' >> $RELEASE_NOTES
	echo '		<link href="rnotes.css" type="text/css" rel="stylesheet" media="all">' >> $RELEASE_NOTES
	echo '	</head>' >> $RELEASE_NOTES
	echo '	<body>' >> $RELEASE_NOTES
	echo "		<h1>Version $HUMAN_VERSION</h1>" >> $RELEASE_NOTES
	echo '	</body>' >> $RELEASE_NOTES
	echo '</html>' >> $RELEASE_NOTES
	
	echo "Wrote release notes skeleton to $RELEASE_NOTES"
fi

echo "Finished"