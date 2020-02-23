# HOSS
Harry's Operating System Software

This is based on two main sources, Nick Blundell’s unfinished book, “Writing a Simple Operating System – from Scratch”, and Carlos Fenollosa's online tutorial, which is specifically tailored to the Mac.  You can find it at https://github.com/cfenollosa/os-tutorial.

For more information and to see my progress, check out the project blog at http://code.culpan.org/hoss/.

### To Build
```
make
```

### To Run in QEMU
By default, it is setup to run in qemu.
```
make run
```

### To Run in Bochs
HOSS can also be booted in Bochs.  To do so (assuming Bochs is installed):
```
make run-bochs
```
After Bochs starts, type `c` to actually run HOSS.

### To Install
```
make image
```
This will make a virtual disk image, hoss-os.img, that you can use in either VMWare or VirtualBox or any other
virtualization solution that supports the i386 architecture.

## Blogging
I've added a blog to this project using the Hexo blogging tool.  This is a
generator of static files.  It is configured to be accessible via the
gh-pages branch.

### To write a new post
```
cd blog
hexo new "<title>"
```
This new post should be committed to the Master branch, not the gh-pages branch.  The deploy step 
below will handle adding the generated form to the gh-pages branch.  This will generate a 
Markdown file, so use your favorite Markdown editor to actually write the post.

### To generate blog
```
make blog-generate
```

### To deploy
```
make blog-deploy
```