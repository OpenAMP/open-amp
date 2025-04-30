# Although RTDs now requires sphinx: configuration: it does not pass it to the
# build command and it still looks for conf.py in the current dir.  It is
# confusing to have a conf.py that will only work in the very specific RTDs
# dir structure so we don't do that.

# So we supply a no-op config file and override the build command to run in the
# openamp-docs dir

# Since we have our own build command now, we could eliminate this file
# completely but that also disables the predefined sphinx steps of
# create_environment and install.  The current setup keeps the build
# environment closer to that used by the real openamp-docs build

print("readthedocs-conf.py does nothing")
