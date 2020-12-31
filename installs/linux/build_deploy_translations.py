#!/usr/bin/python

import re, sys, os, ftplib
from subprocess import check_output
from subprocess import call

Info = '\033[92m'
Warn = '\033[93m'
Fail = '\033[91m'
End = '\033[0m'

print (Info + 'Setting environment variables... ' + End)
SrcDir = '../../src/ComicDesktop'
LRelease = '/usr/bin/lrelease-qt4'
LUpdate = '/usr/bin/lupdate-qt4'
WebsiteDir = '/home/xylamic/Dropbox/Software/Websites/Xylasoft'

SrcDeskFile = SrcDir + "/ComicDesktop.pro"
CSWebDir = WebsiteDir + "/comicseer"
TStatusName = "trans_status.csv"
TStatusFile = CSWebDir + "/" + TStatusName
CSTWebdir = CSWebDir + "/downloads/translations"

print (Info + 'Validating variables...' + End)
if not os.path.exists(SrcDir) :
	print (Fail + 'Source directory does not exist.' + End)
	sys.exit(1)
if not os.path.exists(WebsiteDir) :
	print (Fail + 'Website directory does not exist.' + End)
	sys.exit(1)
if not os.path.exists(WebsiteDir + '/comicseer') :
	print (Fail + 'ComicSeer website directory does not exist.' + End)
	sys.exit(1)

print (Info + 'Updating translations...' + End)
args = [LUpdate, SrcDeskFile]
try :
	output = check_output(args)
except CalledProcessError as e:
		print (Fail + 'Failed to update with error: ' + e.strerror + End)
print (output)

print (Info + 'Compiling translations...' + End)
args = [LRelease, SrcDeskFile]
try :
	output = check_output(args)
except CalledProcessError as e:
		print (Fail + 'Failed to compile with error: ' + e.strerror + End)
print (output)

print (Info + 'Writing compilation status...' + End)
reg = re.compile(	r"Updating.+comicseer_(?P<lang>\w+)\.qm.*\n"
					".*\((?P<fin>\d+)\sfinished.*\s(?P<unfin>\d+)\sunfinished\)\N*\n"
					"(.*Ignored\s(?P<ignore>\d+)\s)?",
					re.MULTILINE)

# find all matches
it = re.finditer(reg, output)
trans_files = []
try :
	file = open(TStatusFile, 'w+')
	for match in it:
		line = match.group('lang') + "," + match.group('fin') + "," + match.group('unfin') + ","
		if match.group('ignore') :
			line = line + match.group('ignore')
		else :
			line = line + "0"
		
		trans_files.append("comicseer_" + match.group("lang"))
		# make sure this is not the sample file
		if not str.startswith(line, "sample") :
			print line  
			file.write(line + "\n")
	file.close()
except IOError:
	print (Fail + "Failed to write status file." + End)

print (Info + 'Deploying updated files to website...' + End)
print ("Copying " + str(len(trans_files)) + " files...")
for trans_file in trans_files :
	print ("Copying " + trans_file + " ts/qm...");
	rc = call(['cp', SrcDir + "/" + trans_file + '.ts', CSTWebdir])
	if rc != 0 :
		print (Fail + "File copy failed." + End)
		sys.exit(1)
	
	rc = call(['cp', SrcDir + "/" + trans_file + '.qm', CSTWebdir])
	if rc != 0 :
		print (Fail + "File copy failed." + End)
		sys.exit(1)

print (Info + 'Uploading translations...' + End)
print ("Connecting to xylasoft.com...")
session = ftplib.FTP('xylasoft.com','comi3023','Storming1!')

session.cwd('comicseer')
print ("Working in '" + session.pwd() + "'")
print ("Uploading trans_status.csv...")
bfile = open(TStatusFile,'rb')
session.storbinary("STOR " + TStatusName, bfile)
bfile.close()

session.cwd('downloads/translations')
print ("Working in '" + session.pwd() + "'")
for trans_file in trans_files :
	print ("Uploading translation " + trans_file + "...")
	bfile = open(CSTWebdir + "/" + trans_file + ".ts", 'rb')
	session.storbinary("STOR " + trans_file + ".ts", bfile)
	bfile.close()

session.quit()

print (Info + "TRANSLATION UPDATE COMPLETE" + End)
