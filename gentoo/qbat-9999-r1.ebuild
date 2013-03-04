
EGIT_REPO_URI="git://repo.or.cz/qbat.git"

inherit cmake-utils eutils git

DESCRIPTION="QBat is a small tool based on Qt4 to display battery stats in system tray."
HOMEPAGE="http://repo.or.cz/qbat.git"
SRC_URI=""

LICENSE="GPL-2.0"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE="debug"

RDEPEND="
	x11-libs/qt-gui:4
	x11-libs/qt-core:4
"

DEPEND="${RDEPEND}
	sys-kernel/linux-headers
	dev-util/cmake"

S=${WORKDIR}/${EGIT_PROJECT}

src_unpack() {
	#check version:
	if [ "$PV" != 9999 ]; then
		EGIT_TREE="v${PV}"
	fi
	git_src_unpack
}

src_install() {
	cmake-utils_src_install
	doman "${S}/res/qbat.1"
}

pkg_postinst() {
	elog "You need a at least kernel version 2.6.24 to use this software"
}
