# Maintainer: "Trilby" <jmcclure [at] cns [dot] umass [dot] edu>
pkgname=leela-git
pkgver=20120605
pkgrel=1
pkgdesc="CLI frontend to poppler-glib of PDF tools"
url="http://github.com/TrilbyWhite/leela.git"
arch=('any')
license=('GPLv3')
depends=('poppler-glib' 'ghostscript')
makedepends=('git')
_gitroot="git://github.com/TrilbyWhite/leela.git"
_gitname="leela"

build() {
	cd "$srcdir"
	msg "Connecting to GIT server...."
	if [ -d $_gitname ] ; then
		cd $_gitname && git pull origin
		msg "The local files are updated."
	else
		git clone $_gitroot $_gitname
	fi
	msg "GIT checkout done or server timeout"
	msg "Starting make..."
	rm -rf "$srcdir/$_gitname-build"
	git clone "$srcdir/$_gitname" "$srcdir/$_gitname-build"
	cd "$srcdir/$_gitname-build"
	make
}

package() {
	cd "$srcdir/$_gitname-build"
	make DESTDIR="$pkgdir/" install
} 
