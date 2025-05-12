Import("env")

# A script to convert firmware.elf to firmware.bin after building

def convert_bin(source, target, env):
    firmware_elf = str(target[0])
    firmware_bin = firmware_elf.replace(".elf", ".bin")
    
    # Use objcopy to convert ELF to BIN
    env.Execute("$OBJCOPY -O binary %s %s" % (firmware_elf, firmware_bin))
    print("Generated %s" % firmware_bin)

# Add a post-action to convert ELF to BIN after a successful build
env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", convert_bin)