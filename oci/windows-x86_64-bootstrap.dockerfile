FROM mcr.microsoft.com/windows/servercore:ltsc2025

SHELL ["powershell", "-Command"]

# Install msys2
RUN powershell -Command "[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12; \
    $uri = $(Invoke-RestMethod -UseBasicParsing https://api.github.com/repos/msys2/msys2-installer/releases/latest | \
    Select -ExpandProperty 'assets' | \
    Select -ExpandProperty 'browser_download_url' | \
    Select-String -Pattern '.sfx.exe$').ToString(); \
    Invoke-WebRequest -UseBasicParsing -Uri $uri -OutFile C:/windows/temp/msys2-base.exe" ; \
    C:/windows/temp/msys2-base.exe ; \
    Remove-Item "C:/windows/temp/msys2-base.exe"

RUN New-Item -ItemType Junction -Path C:/msys64/home/ContainerUser -Target C:/Users/ContainerUser

ENV PATH="$PATH;C:\\msys64\\usr\\bin"

# Install gcc for bootstrapping coreutils and bash. g++ isn't needed
RUN ["bash", "-c", "yes | pacman-key --init && yes | pacman-key --populate msys2 && \
    pacman -S --needed --noconfirm --noprogressbar gcc && \
    rm -rf /var/cache/pacman/pkg/*"]
