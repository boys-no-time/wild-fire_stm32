# wild-fire_stm32
STM32开发
1、将需要上传的项目代码拷贝到此处，

2、执行	git add .  （注：别忘记后面的.，此操作是把当前文件夹下面的文件都添加进来）

3、执行	git commit  -m  ”提交信息”  （注：“提交信息”里面换成你需要，如“first commit”）

4、执行	git push -u origin master   （注：此操作目的是把本地仓库push到github上面）

注：如果提示以下报错：（SSh key 未正确关联）
git@github.com: Permission denied (publickey).
fatal: Could not read from remote repository.

请执行以下指令，关联对应SSH KEY。再执行 push 指令。

ssh-agent bash

ssh-add "/c/Users/Administrator/.ssh/my_github"

ssh-agent bash

ssh-add "/c/Users/Administrator/.ssh/my_gitlab"

ssh-add "/c/Users/Administrator/.ssh/my_github"

ssh-add "/c/Users/linux_pan/.ssh/my_github"

5、重命名远程仓库名称
先在GitHub上Rename仓库名称，然后同步更改本地仓库名称，最后执行下面指令设置仓库地址，将本地与远程仓库对接：
git remote set-url origin https://github.com/boys-no-time/wild-fire_stm32.git

使用下面指令查看一下结果，提示origin URL （fetch）和 origin URL （puch）就是成功了：
git remote -v
