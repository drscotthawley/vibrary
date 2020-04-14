# Vibrary Demo Instructions

## Download & run the Vibrary app

1. For App Binary Version 17 (Sept 19, 2019): Download from https://tinyurl.com/vibrary-017.  
   1. Double-click to unzip the archive
   2. Move the app to /Applications
   3. Double-click to run it, but then you’ll get an error from Mac OS
   4. Go into System Preferences > Security & Privacy, and click the button “Open Anyway”
   5. You should see the Vibrary app come up!

...Otherwise download & build from source. 

## Walkthrough 1: Applying a premade model to your files. (i.e., “Inference”)

1. First let’s select one of the premade “Sort Files” from the dropdown in the upper left. Let’s choose the last one, that says “Sample”

2. In the text field next to the dropdown that says Sample, press the spacebar to see what tags are supported by this Sort File. (Cymbals, Kicks, Snares, Toms)

3. Now find some short audio files on your system that might correspond to these tags. You can do this by...

   1. Searching in Finder or Spotlight, or
   2. Browsing into /Library/Apple/ ... or wherever you know you have such files.

4. Drag the files from the Finder into the Vibrary window, or click the “Add Files” button at the bottom of the Vibrary window. When you do so, you’ll see these files get imported,

   their spectrograms generated and the predicted tag (for this Sort File) assigned by

   Vibrary!

5. In order to which of your files (i.e., which spectrograms) fit a certain tag, type the name

   of the tag into to blank text field next to the Sort File name at the top. For example, if

   you type “Cymbals”, then you’ll see only the files that are tagged as Cymbals.

6. Now what? Can you play these files to listen to them, or drag them from Vibrary into

   another application (e.g. Logic, Pro Tools, Ableton, etc.), or click some sort of “Show in Finder” option? Not yet. These features are next on our To-Do list. For now, the display of these files’ names is all the model does.

7. Try selecting a different premade Sort File from the dropdown at the top. Press spacebar in the text field to see what tags are possible, and drag in other audio files to see how they get classified by the model.

# Walkthrough 2: Creating your own model

*Note: For today’s demo, only one person can train on the server at a time, because the server’s GPU only allows one process at a time. So we’ll need to coordinate “who goes when.” Alternatively, Scott could do the training for everybody, so then you’d mail him your spectrograms & he’d mail you back your trained Sort Files -- in which case, skip step 1 below.*

\1. Setup your ssh key (in order for you to contact the server)

1. *(If you already have a public ssh key, skip to step b.)* In your Mac’s Terminal app,

   type “ssh-keygen” and hit return. Hit return when prompted for a passphrase.

2. Show your public ssh key by typing “cat ~/.ssh/id_rsa.pub”

c. Highlight the output of that command, and press Cmd-C to copy it.

d. Paste this text into an email and send it to: scott.hawley@belmont.edu
 Note: Before you can train your own model, you’ll need to wait for Scott to add your key to the server. In the meantime, you can check out the AES Engineering Brief on Vibrary: https://tinyurl.com/vibrary-ebrief

1. Click the Train icon near the top right. It looks like a note with dots around it:

2. Click “Create Tags”.

3. Make up a name for your first tag, and then either drag-and-drop representative audio

   files into the window, or use the plus “+” icon to pull up a file selection dialog. The more

   audio files you supply, the greater your “Tag Strength”.

4. When you’ve added enough files for this tag to your satisfaction, click “Save Changes” at

   the bottom.

5. Then click “Add Tag” to add another tag, and repeat steps 4 & 5.

6. When you’re ready, click the big “Train Your Vibrary” button at the bottom and watch it

   go! First it creates spectrograms from your audio files*,* then (assuming your ssh key is recognized by the server) these get uploaded to the server, then the model trains, and finally your Sort File is downloaded.

## FAQ

**Q: Which audio file formats are supported?**

A: In theory, wav, aif, caf, aiff, mp3, ogg, and flac -- but only wav has been tested. Not supported (yet) are multi-sample container formats such as Kontakt’s .nkm -- that would be a “Version 2” feature.

**Q: Which sample rates are supported?**

A: Right now, it only looks at raw time-domain samples, and no resampling is performed to convert sample rates. Ideally all audio will get resampled to the same rate, e.g., 44.1 kHz, but this isn’t implemented yet. We may add an option to the Settings panel for which sample rate to use.

**Q: Can I use stereo samples or only mono?**

A: Both. We convert everything to mono by averaging across all channels.

**Q: How much of the audio is used -- i.e., what is the duration used for the spectrograms?**

A: By default, the first 3 seconds of each audio file. We may add Settings options to vary this later.

**Q: Can I assign the same audio file to different tags within the same Sort File?**

A: No. Technically you *can*, but you *shouldn’t*, as it would ‘frustrate’ the model. In order to associate different tags with the same audio file, we allow you to create multiple Sort Files; e.g., one Sort File for sorting on “Instrument,” another Sort File for “Mood,” etc.

**Q: Is this all there is?**

A: It’s a work in progress. We plan to add more options, more UX goodies, etc., but this is where it’s at right now.

**Q: When will Vibrary be officially released?**

A: No date yet.

**Q: Where are the spectrograms stored on my Mac?**

A: In the archive file ~/Library/Caches/Vibrary/Preproc.tar.gz Individual spectrograms within this archive are (by default) NumPy .npz files.

**Q: Where are the Sort Files stored on my Mac?**

A: In ~/Documents/Vibrary, but do not manually alter anything in this directory.

**Q: How do I uninstall Vibrary?**

A: Delete the app, then delete the directories ~/Library/Caches/Vibrary and ~/Documents/Vibrary.
