
(in-package #:cl-user)

(defvar *setup-base-directory* (directory-namestring *load-truename*)
  "Base directory of the pre-configured lisp environment.")

(unless (find-package :asdf)
  #+(or sbcl allegro) (require :asdf)
  #-(or sbcl allegro)
  (let ((*compile-print* nil)
        (*compile-verbose* nil)
        #+cmu (ext:*gc-verbose* nil))
    (handler-bind ((warning #'muffle-warning))
      (load (merge-pathnames #p"asdf/asdf.lisp" *setup-base-directory*)
            :print nil :verbose nil))))

(in-package #:asdf)
(defvar *subdir-search-registry*
  (list (merge-pathnames #p"asdf/" cl-user::*setup-base-directory*))
  "List of directories to search subdirectories within.")
(defvar *subdir-search-wildcard* :wild
  "Value of :wild means search only one level of subdirectories;
   value of :wild-inferiors means search all levels of subdirectories
   (I don't advise using this in big directories!)")
(defun sysdef-subdir-search (system)
  (let ((latter-path (make-pathname :name (coerce-name system)
                                    :directory (list :relative
                                                     *subdir-search-wildcard*)
                                    :type "asd"
                                    :version :newest
                                    :case :local)))
    (dolist (d *subdir-search-registry*)
      (let* ((wild-path (merge-pathnames latter-path d))
             (files (directory wild-path)))
        (when files
          (return (first files)))))))
(pushnew 'sysdef-subdir-search *system-definition-search-functions*)
(in-package #:cl-user)

