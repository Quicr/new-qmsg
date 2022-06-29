use std::ffi::CString;
use std::fs::File;
use std::io::Result;
use std::os::unix::io::{AsRawFd, FromRawFd, RawFd};
use std::time::Duration;

struct FdSet(libc::fd_set);

impl FdSet {
    fn new() -> FdSet {
        unsafe {
            let mut raw_fd_set = std::mem::MaybeUninit::<libc::fd_set>::uninit();
            libc::FD_ZERO(raw_fd_set.as_mut_ptr());
            FdSet(raw_fd_set.assume_init())
        }
    }

    fn set(&mut self, file: &File) {
        unsafe { libc::FD_SET(file.as_raw_fd(), &mut self.0) }
    }

    fn is_set(&mut self, file: &File) -> bool {
        unsafe { libc::FD_ISSET(file.as_raw_fd(), &mut self.0) }
    }

    fn as_mut_ptr(&mut self) -> *mut libc::fd_set {
        &mut self.0
    }
}

impl Drop for FdSet {
    fn drop(&mut self) {
        unsafe { libc::FD_ZERO(&mut self.0) };
    }
}

// Rust doesn't support non-blocking IO, so we have to reach down into libc to get it.
pub fn open(name: &str) -> File {
    let name = CString::new(name).unwrap();
    let fd = unsafe { libc::open(name.as_ptr(), libc::O_NONBLOCK) };
    unsafe { File::from_raw_fd(fd) }
}

// The returned vector indicates which files are ready for reading, among those passed in.
pub fn select(files: &[&mut File], wait: Duration) -> Result<Vec<bool>> {
    // Identify the max file descriptor
    let mut max_fd: RawFd = 0;
    for file in files {
        max_fd = max_fd.max(file.as_raw_fd());
    }

    // Construct an FdSet
    let mut fd_set = FdSet::new();
    for file in files {
        fd_set.set(file);
    }

    // Convert the duration to a timeval
    let mut timeout = libc::timeval {
        tv_sec: wait.as_secs().try_into().unwrap(),
        tv_usec: wait.subsec_micros().try_into().unwrap(),
    };

    // Run select()
    match unsafe {
        libc::select(
            max_fd + 1,
            fd_set.as_mut_ptr(),
            std::ptr::null_mut(),
            std::ptr::null_mut(),
            &mut timeout,
        )
    } {
        -1 => return Err(std::io::Error::last_os_error()),
        _ => {}
    }

    // See which files are ready
    Ok(files.iter().map(|f| fd_set.is_set(f)).collect())
}

pub fn ready(file: &mut File, wait: Duration) -> bool {
    match select(&[file], wait) {
        Err(_) => false,
        Ok(vec) => vec[0],
    }
}
