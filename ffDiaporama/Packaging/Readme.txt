Packaging FEDORA :
------------------
Préparation du PC de packaging :

su -c "yum groupinstall "Development Tools""
su -c "yum install qt-devel ffmpeg-devel SDL-devel SDL_mixer-devel rpmdevtools yum-utils rpmlint"
su -c "yum install qt ffmpeg SDL SDL_mixer exiv2"

