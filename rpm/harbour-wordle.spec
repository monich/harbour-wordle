Name:           harbour-wordle

Summary:        WORDLE game
Version:        1.0.3
Release:        1
License:        BSD
URL:            https://github.com/monich/harbour-wordle
Source0:        %{name}-%{version}.tar.gz

Requires:       sailfishsilica-qt5
Requires:       qt5-qtsvg-plugin-imageformat-svg
Requires:       qt5-qtdeclarative-import-sensors
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  qt5-qttools-linguist

%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

%description
Guess the WORDLE in 6 tries

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5 %{name}.pro
%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
