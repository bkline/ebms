<?php

// $Id$

/**
 * @file
 *
 * Implementation of EBMS Home page.
 */

/**
 * Callback registered with the Drupal menu system.  This is the default
 * landing page, so there are no access restrictions.  Any user who is
 * logged in can view this page.  A visitor who is not logged in is
 * redirected to the login page.
 */
function pdq_ebms_home() {
    if (!user_is_logged_in())
        drupal_goto('login');
    $page = new EbmsHomePage();
    return $page->build();
}
/**
 * Wrapper for logic to build the home pages for the different roles.
 * Keeps our functions out of the global namespace with less ugly
 * syntax than the PHP namespace mechanism provides, and lets us
 * pass around the current state information more easily.
 */
class EbmsHomePage {

    private
        $user,
        $focus,
        $cal,
        $role,
        $url;

    /**
     * Fetch the parameters passed in with the request, along with other
     * information we'll need for building the home page.
     */
    public Function __construct() {
        global $user;
        $current_month = date('Y') . '-' . date('m');
        $this->user = $user;
        $this->focus = Ebms\Util::get_request_value('focus', '1');
        $this->cal = Ebms\Util::get_request_value('cal', $current_month);
        $this->role = user_access('administer') ? 'admin' : 'member';
        $this->url = Ebms\APP_DIR . '/home';
    }

    /**
     * Assemble the information to be shown on the home page, returned
     * as a Drupal 7 render array.  The home page is divided between
     * the main panel ("content") and a right-hand sidebar.  The
     * centerpiece of the page is a set of sliding images representing
     * the primary tasks performed by the current user, with links
     * to the pages for those tasks.  The remainder of the page is
     * made up of boxes of important information.  In theory, those
     * boxes would have been implemented using Drupal's Block API,
     * but at the time of initial implementation of this system
     * the Block API is so poorly documented and buggy that we
     * decided to implement the boxes without it.  The primary
     * benefit provided by use of the Block API is the ability of
     * administrator to control whether and where the blocks
     * are displayed from an administrative drag-and-drop interface.
     * If the blocks on this page need to be rearranged, it should
     * be much more straightforward to adjust the data structures
     * below than to try and work around the bugs in the Block API.
     */
    public function build() {
        drupal_set_title('');
        drupal_set_breadcrumb(array('Home'));
        return array(
            '#theme' => 'page',
            '#type' => 'page',
            '#show_messages' => true,
            'content' => $this->main_panel(),
            'sidebar_second' => $this->right_panel(),
        );
    }

    /**
     * The main panel has three blocks.  The first block is the
     * sliding image block with links to tasks common performed
     * by the current user ("featured" task areas).  The other
     * two blocks are smaller blocks aligned next to each other.
     * The content of the blocks varies depending on the role
     * of the current user.
     */
    private function main_panel() {
        switch ($this->role) {
            case 'member':
                return array(
                    'featured' => $this->featured(),
                    'recent' => $this->recent(),
                    'rosters' => $this->rosters(true),
                );
            case 'admin':
                return array(
                    'featured' => $this->featured(),
                    'recent' => $this->recent(),
                    'admin' => $this->admin(),
                );
        }
    }

    /**
     * The right side panel has a stack of rectangular boxes
     * showing information of current interest.  As with the
     * main panel, the determination of which boxes are shown
     * is based on the role(s) of the current user.
     */
    private function right_panel() {
        $blocks = array(
            'alerts' => $this->alerts(),
            'next meeting' => $this->next_meeting(),
            'calendar' => $this->calendar(),
        );
        if ($this->role == 'admin')
            $blocks['rosters'] = $this->rosters();
        return $blocks;
    }

    /**
     * The centerpiece of the home page is made up of a set of images,
     * only one of which is displayed at any given time.  The images
     * present the illusion of multiple stacked images, each representing
     * one of the primary areas of the system in which the current user
     * would work (for example, Citation Management or Literature
     * Surveillance).  By clicking on the portions of the images which
     * appear to represent another image underneath the one for the area
     * which currently has the focus, she can cause the image to change
     * so that another task area gets the focus.  In the portion of the
     * image for the featured task area links are available for navigating
     * to the pages used to perform the tasks in that portion of the system.
     * In most cases, these links duplicate navigation provided by the
     * main menu system.
     */
    private function featured() {
        $info = $this->featured_info($this->role, $this->focus);
        $id = "featured-{$this->role}-{$this->focus}";
        $prefix = $this->featured_prefix($id);
        foreach ($info['links'] as $link)
            $items[] = $link;
        return array(
            '#theme' => 'item_list',
            '#title' => $info['title'],
            '#items' => $items,
            '#prefix' => $prefix,
            '#suffix' => '</div>',
        );
    }

    /**
     * Assemble the information needed for displaying the image
     * for the home page's main block, as well as the linkable
     * areas of the image for "sliding" to another featured
     * task focus.
     */
    private function featured_prefix($id) {
        $div = "<div class='ebms-featured-block' id='$id'>";
        $url = Ebms\IMAGES_DIR . "/$id.jpg";
        $img = "<img src='$url' usemap='#featured-map' alt='' />";
        $map = array('<map id="featured-map" name="featured-map">');
        if ($this->role == 'admin')
            $alts = array(
                'Citation Management',
                'Literature Surveillance',
                'Summaries',
                'Reports',
            );
        else
            $alts = array(
                'Literature Surveillance',
                'Summaries',
                'Travel',
            );
        $shape = 'shape="rect"';
        $base = "{$this->url}?cal={$this->cal}&focus=";
        for ($i = 0; $i < count($alts); ++$i) {
            $f = $i + 1;
            if ($f != $this->focus) {
                $href = "href='$base$f'";
                if ($f < $this->focus)
                    $x1 = $i * 100 + 10;
                else {
                    $distance_from_last = count($alts) - $i;
                    $x1 = 635 - 100 * $distance_from_last;
                }
                $x2 = $x1 + 85;
                $coords = "coords='$x1,0,$x2,449'";
                $alt = "alt='{$alts[$i]}'";
                $map[] = "<area $href $shape $coords $alt />";
            }
        }
        $map[] = '</map>';
        return $div . $img . implode($map);
    }

    /**
     * Create the home page box for displaying a list of recent
     * activities of interest to the current user.
     * XXX Replace stub activities when the related portions of the
     *     system have been implemented.
     */
    private function recent() {
        $events = array(
            array('Genetics Board meeting time updated.', '02/01/12'),
            array('New genetics articles posted.', '01/30/12'),
            array('New event Genetics Board event posted.', '01/23/12'),
        );
        foreach ($events as $e)
            $items[] = "{$e[0]} <span class='date nobreak'>{$e[1]}</span>";
        return $this->list_block('Recent Activity', 'recent-activity', $items);
    }

    /**
     * Create the box which contains links to administrative tasks.
     * XXX Replace stub URLs when the related portions of the system
     *     have been implemented.
     */
    private function admin() {
        $items = array(
            l('Post a Document', 'docs'),
            l('Manage Groups', 'stub'),
            l('Send a Message', 'stub'),
        );
        $title = 'Administrative Tasks';
        $id = 'admin';
        return $this->list_block($title, $id, $items, true);
    }

    /**
     * Create the home page box containing links to the board rosters.
     * XXX Replace the stub URLs when the roster documents have been
     *     loaded.
     */
    private function rosters($middle=false) {
        $items = array(
            l('Adult Treatment', 'stub'),
            l('Pediatric Treatment', 'stub'),
            l('Supportive and Palliative Care', 'stub'),
            l('Screening and Prevention', 'stub'),
            l('Cancer Genetics', 'stub'),
            l('Cancer Complementary and Alternative Medicine', 'stub'),
            l('Genetics Board Working Groups', 'stub'),
        );
        $title = 'PDQ&reg; Board Rosters';
        $id = 'board-rosters';
        return $this->list_block($title, $id, $items, $middle);
    }

    /**
     * Helper method to create the markup for a single line in
     * the Alerts box on the home page.
     */
    private static function alert($url, $label, $count) {
        $link = l($label, $url);
        return "$link <span class='alert-count'>$count</span>";
    }

    /**
     * Create the home page box showing counts of unread messages,
     * unread forum postings, and (for board members) articles
     * awaiting review or (for board managers) reviews which
     * have not yet been read.
     * XXX Replace stub data when the relevant portions of the
     *     system have been implemented.
     */
    private function alerts() {
        $title = "Alerts <span class='alert-count'>12</span>";
        $items = array(
            EbmsHomePage::alert('stub', 'Messages', 1),
            EbmsHomePage::alert('forum', 'Forum', 5),
            EbmsHomePage::alert('review', 'Reviews', 6),
        );
        return $this->list_block($title, 'alerts', $items);
    }

    /**
     * Create the home page box highlighting the next meeting
     * scheduled for the current user.
     * XXX Replace stub information when the calendar portion
     *     of the system has been implemented.
     */
    private function next_meeting() {
        $now = date_create();
        $then = date_modify($now, '+ 14 days');
        $date = date_format($then, 'F j, Y');
        $time = '10:00 - 11:00 AM ET.';
        $desc = 'Stub Meeting (no events yet)';
        $items = array("<span>$date</span><br />$time<br />$desc");
        return $this->list_block('Next Meeting', 'next-meeting', $items);
    }

    /**
     * Show a small version of the current month's calendar,
     * highlighting days on which events visible to the current
     * user occur, with links to those events.  Allows the user
     * to navigate to different months.
     * XXX The current implementation does not use the third-party
     *     Calendar module, which has not yet been integrated
     *     into the system.  Stub event links are used.
     */
    private function calendar() {
        $cal_date = strtotime($this->cal);
        $cal_year = date('Y', $cal_date);
        $cal_month = date('n', $cal_date);
        $title = date('F Y', $cal_date);
        $today = date('Y-m-d');
        $weeks = $this->get_weeks();
        $rows = array(array('S', 'M', 'T', 'W', 'T', 'F', 'S'));
        foreach ($weeks as $week) {
            $row = array();
            foreach ($week as $date) {
                $day = date('j', $date);
                $month = date('n', $date);
                $dow = date('w', $date);
                $is_weekday = $dow > 0 && $dow < 6;
                if ($month != $cal_month)
                    $row[] = array(
                        'data' => $day,
                        'class' => array('not-this-month'),
                    );
                elseif ($today == date('Y-m-d', $date))
                    $row[] = array(
                        'data' => $day,
                        'class' => array('today'),
                    );
                elseif (rand(1, 5) == 1 && $is_weekday)
                    $row[] = array(
                        'data' => "<a href='stub'>$day</a>",
                        'class' => array('day-has-events'),
                    );
                else
                    $row[] = $day;
            }
            $rows[] = array('data' => $row, 'no_striping' => true);
        }
        $id = 'cal-prev-month';
        $date = mktime(0, 0, 0, $cal_month - 1, 1, $cal_year);
        $cal = date('Y', $date) . '-' . date('m', $date);
        $url = "{$this->url}?focus={$this->focus}&cal=$cal";
        $prev = "<div id='$id'><a href='$url'>&lt;</a></div>";
        $id = 'cal-next-month';
        $date = mktime(0, 0, 0, $cal_month + 1, 1, $cal_year);
        $cal = date('Y', $date) . '-' . date('m', $date);
        $url = "{$this->url}?focus={$this->focus}&cal=$cal";
        $next = "<div id='$id'><a href='$url'>&gt;</a></div>";
        $prefix = $prev . $next;
        return $this->table_block($title, 'calendar', $rows, $prefix);
    }

    /**
     * Helper function to collect a nested array of dates for
     * the selected month's calendar, filling out complete
     * weeks with days at the end of the preceding month and
     * at the beginning of the following month as needed.
     */
    private function get_weeks() {
        $first = strtotime($this->cal . '-01');
        $dow = date('w', $first);
        $month = date('n', $first);
        $year = date('Y', $first);
        $date = mktime(0, 0, 0, $month, 1 - $dow, $year);
        $rows = array();
        while (date('Y-m', $date) <= $this->cal) {
            $row = array();
            while (count($row) < 7) {
                $row[] = $date;
                $m = date('n', $date);
                $d = date('j', $date) + 1;
                $y = date('Y', $date);
                $date = mktime(0, 0, 0, $m, $d, $y);
            }
            $rows[] = $row;
            if (count($rows) > 7) {
                drupal_set_message('bug in get_weeks', 'error');
                break;
            }
        }
        return $rows;
    }

    /**
     * Data controlling which links are displayed in the featured
     * task area of the main panel on the home page, depending
     * the current user's role(s) and on which portion of the
     * panel the user wants to see.
     */
    private static function featured_info($role, $focus) {
        $info = array(
            'admin' => array(
                '1' => array(
                    'title' => 'Citation Management',
                    'links' => array(
                        l('Search Database', 'stub'),
                        l('Review Abstracts', 'stub'),
                        l('Add Citation', 'stub'),
                    ),
                ),
                '2' => array(
                    'title' => 'Literature Surveillance',
                    'links' => array(
                        l('Create A Literature Surveillance Packet', 'review'),
                        l('Edit a Literature Surveillance Packet', 'review'),
                        l('Review Responses to a Literature Surveillance ' .
                            'Packet', 'review'),
                    ),
                ),
                '3' => array(
                    'title' => 'Summaries',
                    'links' => array(
                        l('Post Summary Document', 'summaries'),
                        l('Download and Review Summaries', 'summaries'),
                    ),
                ),
                '4' => array(
                    'title' => 'Reports',
                    'links' => array(
                        l('Literature Surveillance Review Report', 'stub'),
                        l('Travel Reports', 'stub'),
                    ),
                ),
            ),
            'member' => array(
                '1' => array(
                    'title' => 'Literature Surveillance',
                    'links' => array(
                        l('Review the Latest Literature Surveillance',
                            'review'),
                    )
                ),
                '2' => array(
                    'title' => 'Summaries',
                    'links' => array(
                        l('Download and Review Summaries', 'summaries'),
                        l('Submit Summaries with Changes', 'summaries'),
                    )
                ),
                '3' => array(
                    'title' => 'Travel',
                    'links' => array(
                        l('Hotel Reservations', 'travel'),
                        l('Directions to NCI', 'travel'),
                        l('Reimbursements', 'travel'),
                    )
                )
            )
        );
        return $info[$role][$focus];
    }

    /**
     * Assemble the Drupal render array for a home page block containing
     * a list of items.  If it turns out that we need similar blocks
     * on other pages in the system, this functionality will be moved
     * to common.inc and packaged in a separate class.
     */
    private function list_block($title, $id, $items, $middle=false) {
        $class = $middle ? 'ebms-block middle-block' : 'ebms-block';
        return array(
            '#theme' => 'item_list',
            '#title' => $title,
            '#items' => $items,
            '#attributes' => array('class' => 'ebms_block'),
            '#prefix' => '<div class="' . $class . '" id="' . $id . '">',
            '#suffix' => $this->block_bottom_img() . '</div>',
        );
    }

    /**
     * Assemble the Drupal render array for a home page block containing
     * a table.  If it turns out that we need similar blocks on other
     * pages in the system, this functionality will be moved to the
     * common.inc file and packaged in a separate class.
     */
    private function table_block($title, $id, $rows, $extra, $middle=false)
    {
        $class = $middle ? 'ebms-block middle-block' : 'ebms-block';
        $title = '<h3 class="ebms-block-title">' . $title . '</h3>';
        $div = '<div class="' . $class . '" id="' . $id . '">';
        $prefix = $div . $title . $extra;
        return array(
            '#theme' => 'table',
            '#title' => $title,
            '#rows' => $rows,
            '#prefix' => $prefix,
            '#suffix' => $this->block_bottom_img() . '</div>',
        );
    }

    /**
     * Create the markup for the image used for the bottom of one of
     * the EBMS home page boxes.
     */
    private static function block_bottom_img() {
        $url = EbmsHomePage::block_bottom_image();
        return "<img class='ebms-block-bottom' src='$url' alt='' />";
    }

    /**
     * Create the URL for the image used for the bottom of one of the
     * EBMS home page boxes.
     */
    private static function block_bottom_image() {
        return Ebms\IMAGES_DIR . '/box-bottom.png';
    }
}