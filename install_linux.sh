#! /bin/bash
# Must be copied to your release output dir
# and executed sudo from there!

echo
echo "Hard linking AmigaED as /usr/local/bin/amigaed"
sudo cp -l -f AmigaED /usr/local/bin/amigaed
echo "Done."
echo
echo "AmigaED can be invoked by 'amigaed <filename>' now"
echo


