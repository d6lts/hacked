<?php

/**
 * Downloads a project using CVS.
 *
 * @deprecated
 */
class hackedProjectWebCVSDownloader extends hackedProjectWebDownloader {

  var $base_filename = '';

  function hackedProjectWebCVSDownloader(&$project, $base_filename) {
    parent::hackedProjectWebDownloader($project);
    $this->base_filename = $base_filename;
  }

  /**
   * Get information about the CVS location of the project.
   */
  function download_link() {
    $info = array();
    $pinfo = &$this->project->project_info;
    // Special handling for core.
    if ($pinfo['project_type'] == 'core') {
      $info['cvsroot'] = ':pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal';
      $info['module'] = 'drupal';
      $info['checkout_folder'] = $pinfo['name'] . '-' . $pinfo['info']['version'];
      $info['tag'] = $this->cvs_deploy_get_tag($this->base_filename);
    }
    else {
      $info['cvsroot'] = ':pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal-contrib';
      $info['module'] = 'contributions/' . $pinfo['project_type'] . 's/' . $pinfo['name'];
      $info['checkout_folder'] = $pinfo['name'];
      $info['tag'] = $this->cvs_deploy_get_tag($this->base_filename);
    }

    return $info;
  }

  /**
   * Get the CVS tag associated with the given file.
   */
  function cvs_deploy_get_tag($file) {
    $version = '';
    static $available = array();
    $match = array();
    if (empty($version)) {
      // The .info file contains no version data. Find the version based
      // on the sticky tag in the local workspace (the CVS/Tag file).
      $cvs_dir = dirname($file) . '/CVS';
      if (is_dir($cvs_dir)) {
        $tag = '';  // If there's no Tag file, there's no tag, a.k.a. HEAD.
        if (file_exists($cvs_dir . '/Tag')) {
          $tag_file = trim(file_get_contents($cvs_dir . '/Tag'));
          if ($tag_file) {
            // Get the sticky tag for this workspace: strip off the leading 'T'.
            $tag = preg_replace('@^(T|N)@', '', $tag_file);
          }
        }
        $version = $tag;
      }
    }
    // The weird concatenation prevents CVS from 'expanding' this $Name.
    elseif (preg_match('/\$' . 'Name: (.*?)\$/', $version, $match)) {
      $version = trim($match[1]);
    }
    if (empty($version)) {
      $version = 'HEAD';
    }

    return $version;
  }

  function download() {
    $dir = $this->get_destination();
    $release_info = $this->download_link();

    // TODO: Only delete if not a TAG.
    if (file_exists($dir)) {
      if ($this->is_cvs_tag($release_info['tag'])) {
        return $dir;
      }
      else {
        // This is not a CVS tag, so we need to re-download.
        $this->remove_dir($dir);
      }
    }

    if (hacked_cvs_checkout($release_info['cvsroot'], $release_info['module'], $dir, $release_info['checkout_folder'], $release_info['tag'])) {
      return $dir;
    }

    // Something went wrong:
    return FALSE;
  }

  function get_destination() {
    $type = $this->project->project_type;
    $name = $this->project->name;
    $info = $this->download_link();
    // Add in the CVS tag here.
    $version = $this->project->existing_version . '-' . $info['tag'];


    $dir = $this->get_temp_directory() . "/$type/$name";
    // Build the destination folder tree if it doesn't already exists.
    if (!file_prepare_directory($dir, FILE_CREATE_DIRECTORY) && !mkdir($dir, 0775, TRUE)) {
      watchdog('hacked', 'Failed to create temp directory: %dir', array('%dir' => $dir), WATCHDOG_ERROR);
      return FALSE;
    }
    return "$dir/$version";
  }

  function is_cvs_tag($tag) {
    // CVS tags in Drupal are of the form:
    $valid_tags = '@^DRUPAL-[567]--(\d+)-(\d+)(-[A-Z0-9]+)?@';
    return (bool) preg_match($valid_tags, $tag);
  }
}