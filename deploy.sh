#!/bin/bash

processName="imgalign"
targetFolderPath="/var/www/latsic.com/html/imgalign/"
sshLogin="office@latsic.com"

doStartProcess="0"
doStopProcess="0"
doDeployProcess="0"
doPublishProcess="0"
doInstall="0"

isSpa="0"
isDotnet="1"

showUsageAndExit() {
  echo "Unknown parameter: ${!i}"
  echo "Usage: deploy [spa|dotnet=default][stop][publish][deploy][start]"
  exit 1
}

initActions() {
  if [[ "$doStartProcess" == "0" && "$doStopProcess" == "0" && "$doDeployProcess" == "0" && "$doPublishProcess" == "0" ]] ; then
    doDeployProcess="1"
    doPublishProcess="1"
    doStopProcess="1"
    doStartProcess="1"
  fi
}

stopProcess() {

  local processName=$1

  pidOfProcess=`ps -ef | grep -v grep | grep $processName | awk '{print $2}'`

  if [ -z "$pidOfProcess" ]; then
    echo "stop porcess: process $processName not found"
  else
    echo "stop process: $processName, pid $pidOfProcess"
    kill -15 "$pidOfProcess"
  fi
}

createPathIfNotExisting() {
  local path=$1
  mkdir -p $path
}

publishAndCopy() {

  local sshLogin=$1
  local targetPath=$2

  if [[ "$doPublishProcess" == "1" && "$isDotnet" == "1" ]] ; then 
    echo "publishing dotnet app" 
    dotnet publish --configuration Release
  fi

  if [[ "$doPublishProcess" == "1" && "$isSpa" == "1" ]] ; then
    if [[ "$doInstall" == "1" ]] ; then
      echo "installing dependencies for $processName"
      npm install
    fi
    echo "building $processName app"
    npm run build
  fi

  if [[ "$doDeployProcess" == "1"  && "$isDotnet" == "1" ]] ; then
    echo "$sshLogin: deploying app $processName to $targetPath" 
    scp -r ./bin/Release/netcoreapp2.1/publish/. $sshLogin:$targetPath.
  fi

  if [[ "$doDeployProcess" == "1"  && "$isSpa" == "1" ]] ; then
    echo "$sshLogin: deploying app $processName to $targetPath"
    scp -r ./dist/. $sshLogin:$targetPath.
  fi
}

startProcess() {

  local processFolderPath=$1
  local processName=$2 

  cd $processFolderPath
  nohup dotnet $processFolderPath$processName >  $processFolderPath$processName.log 2>&1 & 
  pidOfProcess=`ps -ef | grep -v grep | grep $processName | awk '{print $2}'`
  if [ -z "$pidOfProcess" ]; then
    echo "start process: error verifying process, no pid for $processName found"
  else
    echo "start process: $processName started, pid $pidOfProcess"  
  fi
}

echoRemote() {
  while read IN; do
    if [[ $IN = *[![:space:]]* ]]; then
      # only if non whitespace characters are present. 
      echo "$sshLogin: $IN"  
    fi
  done
}

callRemote() {

  if (($# < 2))
  then
    echo "callRemote: need at least two function arguments, got only $#"  
    echo "usage: callRemote <user@host> <functionName> [<arguments>]"
  else
    local sshLogin=$1
    local funcName=$2 
    local arguments=""
    
    for ((i=3;i<=$#;i++));
    do
      if (("$i"==3)); then
        arguments=${!i}
      else 
        arguments="$arguments ${!i}" 
      fi
    done

    #echo "redirection function call $funcName to $sshLogin, arguments: $arguments"
    { 
       ssh $sshLogin "`declare -f $funcName`; $funcName $arguments; exit"
    } 2>&1 | echoRemote
  fi  
}

addFilePermissions() {

  local processFolderPath=$1

  cd $processFolderPath
  echo "file permissions: adding read permission recursively for all files in $processFolderPath"
  chmod -R o+r $processFolderPath

  find $processFolderPath -type d -exec chmod o+x {} +
}


for ((i=1;i<=$#;i++))
do
  if [[ "${!i}" == "stop" ]] ; then
    doStopProcess="1"
  elif [[ "${!i}" == "start" ]] ; then
    doStartProcess="1"
  elif [[ "${!i}" == "deploy" ]] ; then
    doDeployProcess="1"
  elif [[ "${!i}" == "publish" ]] ; then
    doPublishProcess="1"
  elif [[ "${!i}" == "install" ]] ; then
    doInstall="1"
  elif [[ "${!i}" == "dotnet" ]] ; then
    isDotnet="1"
  elif [[ "${!i}" == "spa" ]] ; then
   isDotnet="0"
   isSpa="1"
  else
    showUsageAndExit
  fi
done

if [[ "$isSpa" == "1" && "$isDotnet" == "1" ]] ; then
  showUsageAndExit
fi

initActions

if [[ "$doStopProcess" = "1" && "$isDotnet" == "1" ]] ; then
  callRemote "$sshLogin" "stopProcess" "$processName"
fi
if [[ "$doDeployProcess" = "1" ]] ; then
  callRemote "$sshLogin" "createPathIfNotExisting" "$targetFolderPath"
fi

publishAndCopy "$sshLogin" "$targetFolderPath"

if [[ "$isSpa" == "1" && "$doDeployProcess" == "1" ]] ; then
  callRemote "$sshLogin" "addFilePermissions" "$targetFolderPath"
fi

if [[ "$doStartProcess" == "1" && "$isDotnet" == "1" ]] ; then
  if [[ "$doStopProcess" == "0" ]] ; then
    echo "Before starting a new process, stoping an existing $processName process (if running)"
    callRemote "$sshLogin" "stopProcess" "$processName"
  fi 
  callRemote "$sshLogin" "startProcess" "$targetFolderPath" "$processName" 
fi

echo "DONE"


