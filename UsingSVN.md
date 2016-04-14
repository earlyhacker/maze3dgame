# Staying up to date #

Периодически надо делать `svn update`, проверить, есть ли новые коммиты, можно на http://code.google.com/p/maze3dgame/updates/list

Чтобы сделать коммит, `svn "--message=Short description" commit`

# Miscellaneous #

`svn add <file>` добавляет файл в репозиторий при следующем коммите.<br>
<code>svn import &lt;dir&gt;</code> добавляет папку в репозиторий.<br>
<code>svn delete &lt;file&gt;</code> удаляет файлы или папки.<br>
<code>svn diff</code> показывает сделанные и не залитые в репозиторий изменения.<br>
<br>
<h1>Resolving conflicts</h1>

<a href='http://ariejan.net/2007/07/04/how-to-resolve-subversion-conflicts/'>Вот эта статья</a> всё весьма неплохо описывает, хотя надо учитывать, что она довольно старая, и с тех пор SVN стал, например, спрашивать, что делать, вот таким образом:<br>
<br>
<pre><code>&gt; svn update<br>
U    settings.cpp<br>
Conflict discovered in 'maze.h'.<br>
Select: (p) postpone, (df) diff-full, (e) edit,<br>
        (mc) mine-conflict, (tc) theirs-conflict,<br>
        (s) show all options: s<br>
<br>
  (e)  edit             - change merged file in an editor<br>
  (df) diff-full        - show all changes made to merged file<br>
  (r)  resolved         - accept merged version of file<br>
<br>
  (dc) display-conflict - show all conflicts (ignoring merged version)<br>
  (mc) mine-conflict    - accept my version for all conflicts (same)<br>
  (tc) theirs-conflict  - accept their version for all conflicts (same)<br>
<br>
  (mf) mine-full        - accept my version of entire file (even non-conflicts)<br>
  (tf) theirs-full      - accept their version of entire file (same)<br>
<br>
  (p)  postpone         - mark the conflict to be resolved later<br>
  (l)  launch           - launch external tool to resolve conflict<br>
  (s)  show all         - show this list<br>
<br>
Select: (p) postpone, (df) diff-full, (e) edit,<br>
        (mc) mine-conflict, (tc) theirs-conflict,<br>
        (s) show all options: p<br>
C    maze.h<br>
G    main.cpp<br>
Conflict discovered in 'draw.cpp'.<br>
Select: (p) postpone, (df) diff-full, (e) edit,<br>
        (mc) mine-conflict, (tc) theirs-conflict,<br>
        (s) show all options: p<br>
C    draw.cpp<br>
Updated to revision 19.<br>
Summary of conflicts:<br>
  Text conflicts: 2<br>
<br>
d:\Dev-Cpp\test\maze\maze3dgame\src&gt;svn help update<br>
update (up): Bring changes from the repository into the working copy.<br>
usage: update [PATH...]<br>
<br>
  If no revision is given, bring working copy up-to-date with HEAD rev.<br>
  Else synchronize working copy to revision given by -r.<br>
<br>
  For each updated item a line will start with a character reporting the<br>
  action taken.  These characters have the following meaning:<br>
<br>
    A  Added<br>
    D  Deleted<br>
    U  Updated<br>
    C  Conflict<br>
    G  Merged<br>
    E  Existed<br>
............<br>
</code></pre>

Если отвечать <b>p</b>, то поведение SVN будет аналогично описанному в статье, также можно написать <b>tc</b>, чтобы все конфликты разрешить в пользу версии в репозитории.