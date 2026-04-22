rm -f romfs/cacert.pem
curl -o romfs/cacert.pem https://curl.se/ca/cacert.pem
echo "SHA256: $(sha256sum romfs/cacert.pem | awk '{print $1}')"
echo "cacert.pem downloaded from Mozilla to romfs/ successfully."