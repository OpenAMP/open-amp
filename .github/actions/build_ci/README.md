# OpenAMP Lib Build Check Docker Action

This action test builds for a specified target.

## Inputs

### `Target`

**Required** the build target. Default `"linux"`.

The supported targets are:

* linux
* generic arm
* zephyr


## Example Usage

Inside a github action use:
```
uses: ./.github/actions/build_ci
with:
  target: linux
```

## Desktop Testing

Right now the directory expectations of the CI script are very messy.
This should be cleaned up in a future PR.

Desktop testing is possible but is likewise messy right now.

One time setup, starting in open-amp directory:
```shell
git clone https://github.com/OpenAMP/libmetal.git
```

A sequence like below will work, change the "zephyr" at the end of the docker command line to "linux" or "generic" for the other working tests
```shell
docker build -t openamp-ci .github/actions/build_ci/ && docker run -it --rm -v$PWD:/prj -w /prj openamp-ci zephyr
sudo rm -rf build* zephyr*
```
