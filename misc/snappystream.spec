Name:		snappystream
Version:	0.1.0
Release:	1%{?dist}
Summary:	Snappy stream C++ library

Group:		System Environment/Libraries
License:	MIT
URL:		http://github.com/hoxnox/snappystream
Source0:	snappystream-0.1.0.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	snappy >= 1.0.0
Requires:	snappy >= 1.0.0

%description

%prep
%setup -q


%build
mkdir build
pushd build
cmake -DCMAKE_BUILD_TYPE=Relese ../
make %{?_smp_mflags}

%install
pushd build
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
/usr/local/include/isnappystream.hpp
/usr/local/include/osnappystream.hpp
/usr/local/include/snappystream.hpp
/usr/local/include/snappystreamcfg.hpp
/usr/local/lib/libsnappystream.a

%changelog

