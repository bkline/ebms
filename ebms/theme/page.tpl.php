<?php

/**
 * @file
 * Custom theme implementation to display a single EBMS page.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['header']: Items for the header region.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 */
  /*dpm($theme_hook_suggestions);*/
$images = Ebms\IMAGES_DIR;
$user_name = $user_photo = '';
$menu_class = 'anon';
if ($logged_in) {
    if (in_array('medical librarian', $user->roles))
        $menu_class = 'librarian';
    elseif (in_array('board member', $user->roles))
        $menu_class = 'member';
    else
        $menu_class = 'manager';
    $user_name = htmlspecialchars($user->name);
    if ($user->picture)
        $uri = Ebms\Util::get_file_uri($user->picture);
    else {
        //$url = $uri = Ebms\IMAGES_DIR . '/avatar-45x45-stroked.png';
        $hash = md5($user->mail);
        $type = 'identicon'; // or mm | monsterid | wavatar | retro
        $uri = "http://www.gravatar.com/avatar/$hash?f=y&s=45&d=$type";
    }
    $photo = array(
        '#type' => 'container',
        '#attributes' => array('id' => 'user-photo'),
        'picture' => array(
            '#theme' => 'image',
            '#path' => $uri,
            '#width' => 45,
            '#height' => 45,
            '#title' => $user_name,
            '#attributes' => array('id' => 'picture'),
        ),
    );
    if ($user->picture)
        $photo['frame'] = array(
            '#theme' => 'image',
            '#path' => Ebms\IMAGES_DIR . "/picframe-45x45.png",
            '#title' => $user_name,
            '#attributes' => array('id' => 'picframe'),
        );
    $photo = drupal_render($photo);
    $profile_link = l(
        'Manage Profile',
        'profile/view/' . $user->uid,
        array(
            'attributes' => array(
                'id' => 'profile-link',
                'title' => 'Profile',
            ),
        )
    );
    $logout_link = l(
        'Log Out',
        'user/logout',
        array(
            'attributes' => array(
                'id' => 'logout-link',
                'title' => 'Log Out',
            ),
        )
    );
}
?>

    <div id="page-wrapper">
      <div id="page">

        <!-- Banner strip with approved NCI logo -->
        <div id="banner">
          <div class="section clearfix">
            <img src="<?php print $images ?>/nciminibannermaster_960.gif"
                 alt="National Cancer Institute" usemap="#bannermap" />
            <map id="bannermap" name="bannermap">
              <area href="http://www.cancer.gov" shape="rect"
                    coords="10,5,268,33" alt="NCI" />
              <area href="http://www.nih.gov" shape="rect"
                    coords="684,18,844,27" alt="NIH" />
              <area href="http://www.cancer.gov" shape="rect"
                    coords="855,18,950,30" alt="NCI" />
            </map>
          </div>
        </div> <!-- /#banner -->

        <!-- Header with site title and identification of logged-in user -->
        <div id="header">
          <div class="section clearfix">
            <div id="name-and-slogan">
              <div id="site-name">
                <a href="<?php print $front_page; ?>"
                   title="Home" rel="home">EBMS</a>
              </div>
              <div id="site-slogan">PDQ Editorial Board Management System</div>
            </div> <!-- /#name-and-slogan -->
<?php if ($logged_in) { ?>
            <div id="user-profile">
<?php if ($photo) { ?>
             <?php echo $photo; ?>
<?php } /* if photo */ ?>
              <span id="user-name"><?php print $user_name; ?></span>
              <div id="profile-and-logout">
                <span id="profile-and-logout-marker">&#9660;</span>
                <ul id="profile-and-logout-menu">
                  <li><?php print $profile_link; ?></li>
                  <li><?php print $logout_link; ?></li>
                </ul>
              </div>
            </div> <!-- /#user-profile -->
<?php } /* if logged in */ ?>
          </div>
        </div> <!-- /#header -->

<?php if ($logged_in) { ?>
        <!-- Navigation main menu -->
        <div id="nav" class="<?php print $menu_class; ?>">
          <div class="section clearfix">
<?php
          if (true || $user_name == 'David Ryan' || $user_name == 'Bob Kline')
              print render($page['ebms_menu']);
          else
          print theme('links__system_main_menu', array(
                  'links' => $main_menu,
                  'attributes' => array(
                      'id' => 'main-menu',
                      'class' => array('links', 'inline', 'clearfix')
                  ),
              )
          );
?>
          </div>
        </div> <!-- /#navigation -->

<?php if ($breadcrumb) { ?>
        <div id="breadcrumb" class="clearfix"><?php print $breadcrumb; ?></div>
<?php } /* if breadcrumb */ ?>
<?php } /* if logged in*/ else { /* not logged in */ ?>
        <div id="login-image-wrapper">
          <img src="<?php print $images ?>/login.jpg"
               alt="PDQ Editorial Board Management System" />
        </div>
<?php } /* not logged in */ ?>

<?php if ($messages) { ?>
        <div id="message-wrapper">
<?php print $messages; ?>
        </div>
<?php } /* if messages */ ?>
        <!-- Payload for the page, possibly divided into multiple columns -->
        <div id="main-wrapper">
          <div id="main" class="clearfix">
<?php if (true || $logged_in) { ?>
            <div id="content" class="column">
              <div class="section">
<?php if (false && $page['highlighted']) { ?>
                <div id="highlighted">
<?php print render($page['highlighted']); ?>
                </div>
<?php } /* if highlighted page */ ?>
                <a id="main-content"></a>
<?php print render($title_prefix); ?>
<?php if ($title) { ?>
                <h1 class="title" id="page-title"><?php print $title; ?></h1>
<?php } /* if title */ ?>
<?php print render($title_suffix); ?>
<?php if ($tabs) { ?>
                <div class="tabs"><?php print render($tabs); ?></div>
<?php } /* if tabs */ ?>
<?php print render($page['help']); ?>
<?php if ($action_links) { ?>
                <ul class="action-links">
<?php print render($action_links); ?>
                </ul>
<?php } /* if action links */ ?>
<?php print render($page['content']); ?>
<?php print $feed_icons; ?>
              </div>
            </div> <!-- /#content -->
<?php } /* if logged in */ ?>

<?php if ($page['sidebar_first']) { ?>
            <div id="sidebar-first" class="column sidebar">
              <div class="section">
<?php print render($page['sidebar_first']); ?>
              </div>
            </div> <!-- /#sidebar-first -->
<?php } /* if first sidebar */ ?>

<?php if ($page['sidebar_second']) { ?>
            <div id="sidebar-second" class="column sidebar">
              <div class="section">
<?php print render($page['sidebar_second']); ?>
              </div>
            </div> <!-- /#sidebar-second -->
<?php } /* if second sidebar */ ?>

          </div> <!-- /#main -->
        </div> <!-- /#main-wrapper -->

        <!-- Footer -->
        <div id="footer">
          <div class="section">
            <?php print l('EBMS Home', 'home') ?> |
            <?php print l('Contact Us', 'stub') ?> |
            <?php print l('Policies', 'stub') ?> |
            <?php print l('Accessibility', 'stub') ?> |
            <?php print l('Viewing Files', 'alantest') ?> |
            <?php print l('FOIA', 'clear-theme-cache') ?> |
            <?php print l('Site Help', 'admin') ?> |
            <?php print l('Site Map', 'clear-all-caches') ?> <br />
            <a href="http://www.hhs.gov" target="_blank"
              ><img src="<?php print $images; ?>/hhs.png" alt="HHS" />
            </a> &nbsp;
            <a href="http://www.nih.gov" target="_blank"
              ><img src="<?php print $images; ?>/nih.png" alt="NIH" />
            </a> &nbsp;
            <a href="http://www.cancer.gov" target="_blank"
              ><img src="<?php print $images; ?>/nci-logo.png" alt="NCI" /></a>
            <a href="http://www.usa.gov" target="_blank"
              ><img src="<?php print $images; ?>/usa.png" alt="USA GOV" /></a>
          </div>
        </div> <!-- /#footer -->
      </div> <!-- /#page -->
    </div> <!-- /#page-wrapper -->
