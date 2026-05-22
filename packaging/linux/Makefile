all: snap appimage
.PHONY: all snap appimage

snap:
	mkdir -p snap/gui
	cp ../../icons/bossa-logo-64.png snap/gui
	cp org.bossa.Bossa.desktop snap/gui/bossa.desktop
	sed -i 's|Icon=.*|Icon=$${SNAP}/meta/gui/bossa-logo-64.png|' snap/gui/bossa.desktop
	cat snap/gui/bossa.desktop
	VERSION=$(shell wget -q "https://api.github.com/repos/cabezno/BOSSA/releases/latest"  -O - | grep tag_name | cut -d'"' -f4 | sed 's|^v||')
	FILENAME="https://github.com/cabezno/BOSSA/releases/download/v${VERSION}/bossa-linux-x86_64-$(shell echo $VERSION | tr -d .).txz"
	sed "s/\$VERSION/${VERSION}/ ; s/\$FILENAME/${FILENAME}/" snapcraft.yaml.in > snap/snapcraft.yaml
	snapcraft --use-lxd

appimage: appimage/appimage.yml
	mkdir -p Bossa
	echo $(shell wget -q "https://api.github.com/repos/cabezno/BOSSA/releases/latest"  -O - | grep tag_name | cut -d'"' -f4 | sed 's|^v||') > Bossa/VERSION
	wget -N https://raw.githubusercontent.com/probonopd/AppImages/master/pkg2appimage
	bash -ex pkg2appimage appimage/appimage.yml

clean:
	rm -rf out pkg2appimage Bossa
