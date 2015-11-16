Steps to build

1. Install node
2. Install python 2.7
3. `npm install -g node-gyp`
4. `npm install`
5. Run this to download node-gyp dist for electron (ignore errors due to the missing binding.gyp file)
`node-gyp configure --target=0.33.7 --arch=x64 --dist-url=https://atom.io/download/atom-shell`
6. Set "ElectronGypHome" env var to where the node-gyp dist for electron was installed, e.g. C:\Users\Volodymyr\.node-gyp\0.33.7
7. Now you can build the project in MSVC normally
